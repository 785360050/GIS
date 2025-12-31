#include "DEM.h"

#include <SpatialReference.h>
#include <Point.h>

#include <ShapefileInfo.h>
#include <SceneGraphicsView.h>
#include <Surface.h>
#include <SceneView.h>      // 3D 视图
#include <RendererSceneProperties.h> // 3D 渲染属性
#include <Envelope.h>          // 包围盒
#include <SymbolTypes.h>
#include <LayerListModel.h>
#include <SceneViewTypes.h>
#include <ElevationSourceListModel.h>
#include <MapTypes.h>

#include <Scene.h>
#include <Error.h>
#include <Basemap.h>
#include <ShapefileFeatureTable.h>
#include <FeatureLayer.h>
#include <SimpleRenderer.h>
#include <SimpleFillSymbol.h>
#include <SimpleLineSymbol.h>
#include <RendererSceneProperties.h>
#include <Camera.h>
#include <BackgroundGrid.h>
#include <Viewpoint.h>
#include <TaskWatcher.h>
#include <ArcGISTiledLayer.h>
#include <RasterLayer.h>
#include <Raster.h>
#include <RasterElevationSource.h>
#include <GeometryEngine.h>

#include <MinMaxStretchParameters.h>
#include <GeoPackage.h>
#include <FeatureTable.h>
#include <UniqueValue.h>
#include <UniqueValueRenderer.h>
#include <UniqueValueListModel.h>
#include <ArcGISSceneLayer.h>
#include <LayerSceneProperties.h>

#include <QDir>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QFuture>
#include <QApplication>
#include <QUrl>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QStringList>

#include <iostream>

#include "Signal_Proxy.hpp"

using namespace Esri::ArcGISRuntime;

#define Demo

DEM::DEM(QWidget* parent /*=nullptr*/)
    : QMainWindow(parent)
{
    // Create a scene using the ArcGISTerrain BasemapStyle
    // m_scene = new Scene(BasemapStyle::ArcGISTerrain, this);
    // m_scene = new Scene(BasemapStyle::ArcGISImagery, this);

    // Create a scene view, and pass in the scene
    // m_sceneView = new SceneGraphicsView(m_scene, this);

    m_scene = new Scene(this);
    m_scene->setBasemap(nullptr); // 离线模式，清空在线底图
    m_sceneView = new SceneGraphicsView(m_scene, this);
    m_sceneView->setSunLighting(LightingMode::LightAndShadows);
    m_sceneView->setSunLighting(LightingMode::NoLight);

    Esri::ArcGISRuntime::SpatialReference spatialReference = m_scene->spatialReference();
    std::cout << "当前空间参考：" << spatialReference.wkText().toStdString()<<std::endl;

    Load_Resource();

    // set the sceneView as the central widget
    setCentralWidget(m_sceneView);
}

QString FILE_PATH_BASE_MAP = "../../Resource/Basemap/cliped.tif";
QString FILE_PATH_ELEVATION = "../../Resource/Elevation/output_hh.tif";
QString FILE_PATH_GEO_PACKAGE = "../../Resource/Building/test1.gpkg";
void DEM::Load_Resource()
{
    if (!QFile::exists(FILE_PATH_BASE_MAP) || !QFile::exists(FILE_PATH_ELEVATION))
    {
        QMessageBox::critical(this, "文件缺失", "请检查代码中的 TIF 文件路径！");
        return;
    }

    //必须先加载高程，再底图，最后建筑
    _Load_Elevation();
    _Load_Basemap();

}

void DEM::_Load_Elevation()
{
    // 先加载高程数据
    Surface* surface = new Surface(this);
    RasterElevationSource* elevSrc = new RasterElevationSource(QStringList{FILE_PATH_ELEVATION}, this);
    surface->elevationSources()->append(elevSrc);
    // 允许地下导航，防止相机稍微低一点就黑屏
    surface->setNavigationConstraint(NavigationConstraint::None);
    // 设置不透明度 (防止穿透看到背景色)
    surface->setOpacity(1.0f);
    // 【夸张地形】为了测试高程是否生效，建议先设为 3.0 或 5.0
    // 如果是 1.0，平原地区可能看不出来
    surface->setElevationExaggeration(1.0f);
    // 应用到场景
    m_scene->setBaseSurface(surface);
}

void DEM::_Load_Basemap()
{
    // 加载底图
    Raster* mapRaster = new Raster(FILE_PATH_BASE_MAP, this);
    // RasterLayer* mapLayer = new RasterLayer(mapRaster, this);
    mapLayer = new RasterLayer(mapRaster, this);
    // 连接信号：当地图加载完成后执行
    // connect(mapLayer, &Layer::doneLoading, this, &DEM::Handle_MapLayer);
    connect(mapLayer, &Layer::doneLoading, this, [&](const Esri::ArcGISRuntime::Error &e)
            {
                if (!e.isEmpty())
                {
                    qDebug() << "底图加载失败" << e.message();
                    return;
                }

                Handle_MapLayer();
            });

    Basemap* basemap = new Basemap(mapLayer, this);
    m_scene->setBasemap(basemap);
}

void DEM::_Load_GeoPackage()
{
    //加载建筑数据
    if (!QFile::exists(FILE_PATH_GEO_PACKAGE))return;

    // 1. 打开 GPKG
    GeoPackage* gpkg = new GeoPackage(FILE_PATH_GEO_PACKAGE, this);

    connect(gpkg, &GeoPackage::doneLoading, this, [this, gpkg](const Error& e)
            {
                if (!e.isEmpty())
                {
                    qDebug() << "GPKG 加载失败:" << e.message();
                    return;
                }

                Handle_Building(gpkg);
            });

    gpkg->load();

}


void DEM::Handle_MapLayer()
{

    _Load_GeoPackage();
    // 获取图层范围并设置镜头
    Set_Camera_Extent(mapLayer->fullExtent());
}

void DEM::Handle_Building(Esri::ArcGISRuntime::GeoPackage *gpkg)
{


    // 2. 获取第一个图层
    if (gpkg->geoPackageFeatureTables().isEmpty()) return;
    FeatureTable* table = (FeatureTable*)gpkg->geoPackageFeatureTables().at(0);

    // 3. 创建 FeatureLayer
    m_buildingLayer = new FeatureLayer(table, this);

    // 4. static不显示
    m_buildingLayer->setRenderingMode(FeatureRenderingMode::Dynamic);

    LayerSceneProperties layerProps(SurfacePlacement::Relative);
    m_buildingLayer->setSceneProperties(layerProps);

    UniqueValueRenderer* typeRenderer = new UniqueValueRenderer(this);

    QStringList fieldNames = typeRenderer->fieldNames();  // OSM 数据常用 "building"
    fieldNames.append("building");
    typeRenderer->setFieldNames(fieldNames);
    // 定义颜色和符号创建辅助函数
    auto createFillSymbol = [](const QColor& color, QObject* parent = nullptr) -> SimpleFillSymbol* {
        SimpleFillSymbol* symbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, color, parent);
        SimpleLineSymbol* outline = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(50,50,50), 1.0, parent);
        symbol->setOutline(outline);
        return symbol;
    };

    QColor residentialColor(255, 140, 0, 255);   // 深橙 - 居住类  255不透明  0完全透明
    QColor educationColor(30, 144, 255, 255);    // 道奇蓝 - 教育类
    QColor commercialColor(50, 205, 50, 255);    // 酸橙绿 - 商业类
    QColor publicColor(138, 43, 226, 255);       // 蓝紫 - 公共设施类
    QColor otherColor(169, 169, 169, 255);       // 深灰 - 其他

    // ========== 居住类 ==========
    QStringList residentialTypes =
        {
            "apartments", "dormitory", "residential", "house", "detached",
            "semidetached_house", "static_caravan", "bungalow", "hut"
        };

    for (const QString& type : residentialTypes)
    {
        QVariantList values;
        values << type;  // 必须包装成 QVariantList

        UniqueValue* uv = new UniqueValue(
            "居住类",                         // label（图例显示）
            QString("居住建筑：%1").arg(type), // description
            values,                           // values
            createFillSymbol(residentialColor, this), // symbol
            this                              // parent
            );
        typeRenderer->uniqueValues()->append(uv);
    }

    // ========== 教育类 ==========
    QStringList educationTypes = {"university", "school", "college", "kindergarten"};
    for (const QString& type : educationTypes)
    {
        QVariantList values;
        values << type;

        UniqueValue* uv = new UniqueValue(
            "教育类",
            QString("教育设施：%1").arg(type),
            values,
            createFillSymbol(educationColor, this),
            this
            );
        typeRenderer->uniqueValues()->append(uv);
    }

    // ========== 商业/办公类 ==========
    QStringList commercialTypes =
        {
            "office", "commercial", "retail", "supermarket", "warehouse",
            "industrial", "industry", "service"
        };
    for (const QString& type : commercialTypes)
    {
        QVariantList values;
        values << type;

        UniqueValue* uv = new UniqueValue(
            "商业类",
            QString("商业建筑：%1").arg(type),
            values,
            createFillSymbol(commercialColor, this),
            this
            );
        typeRenderer->uniqueValues()->append(uv);
    }

    // ========== 公共设施类 ==========
    QStringList publicTypes =
        {
            "hospital", "civic", "transportation", "train_station", "railway",
            "dam", "stadium", "sports_centre", "grandstand", "pavilion",
            "public", "mosque", "temple", "church", "hangar", "carport",
            "shed", "gatehouse", "greenhouse"
        };
    for (const QString& type : publicTypes)
    {
        QVariantList values;
        values << type;

        UniqueValue* uv = new UniqueValue(
            "公共设施",
            QString("公共建筑：%1").arg(type),
            values,
            createFillSymbol(publicColor, this),
            this
            );
        typeRenderer->uniqueValues()->append(uv);
    }

    // ========== 其他类（默认符号）==========
    SimpleFillSymbol* defaultSymbol = createFillSymbol(otherColor, this);
    typeRenderer->setDefaultSymbol(defaultSymbol);
    typeRenderer->setDefaultLabel("其他");

    RendererSceneProperties sceneProps = typeRenderer->sceneProperties();
    sceneProps.setExtrusionMode(ExtrusionMode::BaseHeight);
    //这里calc_height是我自己在QGIS中设置的一个属性
    sceneProps.setExtrusionExpression("[calc_height]");

    typeRenderer->setSceneProperties(sceneProps);
    m_buildingLayer->setRenderer(typeRenderer);

    m_scene->operationalLayers()->append(m_buildingLayer);
    qDebug() << "GeoPackage 建筑加载成功！";
}

void DEM::Set_Camera_Extent(const Esri::ArcGISRuntime::Envelope &ext)
{
    if (!ext.isValid() || ext.isEmpty())
    {
        qDebug() << "图层范围无效，无法定位";
        return;
    }

    qDebug() << "底图加载成功";
    // 获取原始中心点 (UTM 投影坐标，X=30多万, Y=300多万)
    Point centerUtm = ext.center();
    // Camera 类必须使用经纬度才能正确飞行，直接传米会导致飞到外太空
    Geometry projectedGeo = GeometryEngine::project(centerUtm, SpatialReference::wgs84());
    Point centerWgs84 = (Point)projectedGeo;
    // 乘以 1.5 是为了让相机离得稍微远一点，能看清全貌。
    double targetAlt = (ext.width() * 1.5) + 2000.0;
    Camera cam(centerWgs84.y(), centerWgs84.x(), targetAlt + 3000, 0, 0, 0);
    m_sceneView->setViewpointCameraAsync(cam, 2.0f);
}




