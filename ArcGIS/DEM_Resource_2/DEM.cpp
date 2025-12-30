// Copyright 2025 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

// Other headers
#include "DEM.h"

// C++ API headers
#include "ArcGISTiledElevationSource.h"
#include "ElevationSourceListModel.h"
#include "MapTypes.h"
#include "RasterElevationSource.h"
#include "Scene.h"
#include "SceneGraphicsView.h"
#include "Surface.h"
#include "Envelope.h"
#include "Viewpoint.h"
#include "SpatialReference.h"
#include "Error.h"
#include "Point.h"
#include "Camera.h"

#include <QUrl>
#include <QFileInfo>
#include <QFuture>

#include <iostream>


// C++ API headers
#include "ArcGISTiledElevationSource.h"
#include "ElevationSourceListModel.h"
#include "MapTypes.h"
#include "SceneGraphicsView.h"
#include "Surface.h"
#include <SpatialReference.h>
#include <Point.h>
#include <QFuture>
#include <QUrl>
#include <ShapefileInfo.h>
#include <SceneView.h>      // 3D 视图
#include <RendererSceneProperties.h> // 3D 渲染属性
#include <Envelope.h>          // 包围盒
#include <SymbolTypes.h>
#include <LayerListModel.h>
#include <SceneViewTypes.h>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDir>
// ArcGIS SDK 头文件
#include <Scene.h>
#include <Basemap.h>
#include <ShapefileFeatureTable.h>
#include <FeatureLayer.h>
#include <SimpleRenderer.h>
#include <SimpleFillSymbol.h>
#include <SimpleLineSymbol.h>
#include <RendererSceneProperties.h>
#include <Camera.h>
#include <QApplication>
#include <BackgroundGrid.h>
#include <Viewpoint.h>
#include <TaskWatcher.h>
#include <ArcGISTiledLayer.h>
#include <RasterLayer.h>
#include <Raster.h>
#include <RasterElevationSource.h>
#include <GeometryEngine.h>
#include <QFutureWatcher>
#include <QMouseEvent>
#include <MinMaxStretchParameters.h>
#include <GeoPackage.h>
#include <FeatureTable.h>
#include <UniqueValue.h>
#include <UniqueValueRenderer.h>
#include <QStringList>
#include <UniqueValueListModel.h>
#include <RGBRenderer.h>
#include <PercentClipStretchParameters.h>
#include <ArcGISSceneLayer.h>

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

    // 假设您有一个场景（Scene）对象
    Esri::ArcGISRuntime::SpatialReference spatialReference = m_scene->spatialReference();

    // 输出空间参考的信息
    std::cout << "当前空间参考：" << spatialReference.wkText().toStdString()<<std::endl;

    loadDataAndPrintExtent();
    // useGeoPackageBuild();

    // set the sceneView as the central widget
    setCentralWidget(m_sceneView);
}


void DEM::loadDataAndPrintExtent()
{
    if (!QFile::exists(m_mapTifPath) || !QFile::exists(m_elevTifPath))
    {
        QMessageBox::critical(this, "文件缺失", "请检查代码中的 TIF 文件路径！");
        return;
    }
    //必须先加载高程
    loadElevation();
    // 加载底图
    Raster* mapRaster = new Raster(m_mapTifPath, this);
    RasterLayer* mapLayer = new RasterLayer(mapRaster, this);
    // 连接信号：当地图加载完成后执行
    connect(mapLayer, &Layer::doneLoading, this, [this, mapLayer](const Error& e)
            {
                if (!e.isEmpty()) {
                    qDebug() << "底图加载失败" << e.message();
                    return;
                }

                // 获取底图的栅格数据
                Raster* raster = mapLayer->raster();
                if (!raster) return;

                // 确保 Raster 本身也加载完成了
                // Layer 加载完不代表 Raster 的元数据（如波段数）已经准备好
                // 如果 Raster 还没 Loaded，我们强制让它 Load，或者等待
                if (raster->loadStatus() != LoadStatus::Loaded)
                {
                    connect(raster, &Raster::doneLoading, this, [this, mapLayer, raster](const Error& err)
                            {
                                if (err.isEmpty())
                                {
                                    // Raster 加载完后，再次尝试应用渲染
                                    applyRenderer(mapLayer, raster);
                                }
                            });
                    raster->load(); // 触发加载
                } else {
                    // 如果已经 Ready，直接应用
                    applyRenderer(mapLayer, raster);
                }

                // useEsriOnlineBuild();
                useGeoPackageBuild();
                // 获取图层范围并设置镜头
                setCameraExtent(mapLayer->fullExtent());
            });

    Basemap* basemap = new Basemap(mapLayer, this);
    m_scene->setBasemap(basemap);

}

void DEM::loadElevation()
{
    // 先加载高程数据
    Surface* surface = new Surface(this);
    RasterElevationSource* elevSrc = new RasterElevationSource(QStringList{m_elevTifPath}, this);
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

void DEM::applyRenderer(RasterLayer* layer, Raster* raster)
{
    // 准备拉伸参数：去除最亮和最暗的 2% 噪点 (解决灰色/黑暗问题)
    PercentClipStretchParameters stretchParams(2.0, 2.0);
    RGBRenderer* newRenderer = new RGBRenderer(
        stretchParams,
        QList<int>{0, 1, 2},
        QList<double>{1.0, 1.0, 1.0}, // Gammas
        true, // estimateStats: 必须为 true，否则是一片黑/灰
        this
        );
    layer->setRenderer(newRenderer);
}

void DEM::useEsriOnlineBuild()
{
    QUrl newYorkUrl("https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/Buildings_Philadelphia/SceneServer");

    ArcGISSceneLayer* sceneLayer = new ArcGISSceneLayer(newYorkUrl, this);
    connect(sceneLayer, &Layer::doneLoading, this, [this, sceneLayer](const Error& e){
        if (e.isEmpty()) {
            qDebug() << "纽约 3D 建筑加载成功！";

            // 【关键】加载成功后，必须飞过去才能看到！
            // 否则你在原来的位置（比如上海）是什么都看不到的
            if (sceneLayer->fullExtent().isValid()) {
                setCameraExtent(sceneLayer->fullExtent());
            }
        }
        else {
            qDebug() << "加载失败：" << e.message() << " | 错误详情：" << e.additionalMessage();
        }
    });
    m_scene->operationalLayers()->append(sceneLayer);
}

#include <LayerSceneProperties.h>
void DEM::useGeoPackageBuild()
{
    //加载建筑数据
    if (!QFile::exists(gpkgPath))return;

    // 1. 打开 GPKG
    GeoPackage* gpkg = new GeoPackage(gpkgPath, this);

    connect(gpkg, &GeoPackage::doneLoading, this, [this, gpkg](const Error& e)
            {
                if (!e.isEmpty())
                {
                    qDebug() << "GPKG 加载失败:" << e.message();
                    return;
                }

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
            });

    gpkg->load();

}

void DEM::setCameraExtent(const Esri::ArcGISRuntime::Envelope &ext)
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



