#include "Shapefile.h"

// C++ API headers
#include "ArcGISTiledElevationSource.h"
#include "ElevationSourceListModel.h"
#include "MapTypes.h"
#include "RasterElevationSource.h"
#include "Scene.h"
#include "SceneGraphicsView.h"
#include "LayerListModel.h"
#include "FeatureLayer.h"
#include "ShapefileFeatureTable.h"
#include "Surface.h"
#include "Envelope.h"
#include "Viewpoint.h"
#include "SpatialReference.h"
#include "Error.h"
#include "Point.h"
#include "Camera.h"
#include "GeometryEngine.h"

#include <QUrl>
#include <QFileInfo>
#include <QFuture>

#include <iostream>
#include <filesystem>

#include "Signal_Proxy.hpp"

using namespace Esri::ArcGISRuntime;

Shapefile::Shapefile(QWidget* parent /*=nullptr*/)
    : QMainWindow(parent)
{
    this->setWindowTitle("Shapefile");

    // 1. 创建 Scene （使用 ArcGISTerrain 底图）
    m_scene = new Scene(BasemapStyle::ArcGISTerrain, this);

    // 2. 创建 SceneView
    m_sceneView = new SceneGraphicsView(m_scene, this);

    // create a new elevation source from Terrain3D rest service
    ArcGISTiledElevationSource *elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

    auto surface = m_scene->baseSurface();
    surface->elevationSources()->append(elevationSource);// add the elevation source to the scene to display elevation
    m_scene->setBaseSurface(surface);

    // QString file_path="/mnt/Github/GIS/ArcGIS/DEM_Resource/Resource/terrain_data_directory/general_shapefiles_and_images/AFR_voidMap.shp";
    // QString file_path="/mnt/Github/GIS/ArcGIS/DEM_Resource/Resource/terrain_data_directory/general_shapefiles_and_images/tile_index.shp";
    // QString file_path="/mnt/Github/GIS/ArcGIS/DEM_Resource/Resource/terrain_data_directory/general_shapefiles_and_images/Source_SRTM.shp";
    // QString file_path="/mnt/Github/GIS/ArcGIS/DEM_Resource/Resource/terrain_data_directory/general_shapefiles_and_images/Source_RADARSAT.shp"; // fine
    QString file_path="/mnt/Github/GIS/ArcGIS/DEM_Resource/Resource/上海/上海.shp"; // fine
    // QString file_path="/mnt/Github/GIS/ArcGIS/DEM_Resource/Resource/上海_Floor/上海_ExportFeatures.shp"; // error

    auto* table = new ShapefileFeatureTable(file_path, this);
    auto* layer = new FeatureLayer(table, this);

    m_scene->operationalLayers()->append(layer);

    if(!std::filesystem::exists(file_path.toStdString()))
        qDebug()<<"file not found";

    connect(layer, &FeatureLayer::doneLoading, this,
            [this, layer](const Error& error)
            {
                if (!error.isEmpty())
                {
                    std::cout << "Shapefile 加载失败："<< error.message().toStdString() << std::endl;
                    return;
                }

                Envelope extent = layer->fullExtent();

                // 如果 shapefile 没有坐标系，ArcGIS 可能给一个空范围
                if (extent.isEmpty())
                {
                    std::cout << "Shapefile 为空或无范围信息" << std::endl;
                    return;
                }

                // 统一投影到 WGS84
                Envelope wgs84Extent = GeometryEngine::project(extent, SpatialReference::wgs84()).extent();
                Point center = wgs84Extent.center();

                std::cout << "Shapefile 范围中心："
                          << center.x() << ", "
                          << center.y() << std::endl;

                // 创建相机并飞过去
                Camera camera(center.y(), center.x(), 30000, 0, 60, 0);
                m_sceneView->setViewpointCameraAsync(camera);

                emit Signal_Proxy::Instance().Sync_Viewpoint(camera);
            });

    // 6. 设置到窗口
    setCentralWidget(m_sceneView);
}


Shapefile::~Shapefile() = default;
