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

#include "Signal_Proxy.hpp"

using namespace Esri::ArcGISRuntime;

// #define Demo

DEM::DEM(QWidget* parent /*=nullptr*/)
    : QMainWindow(parent)
{
    // Create a scene using the ArcGISTerrain BasemapStyle
    m_scene = new Scene(BasemapStyle::ArcGISTerrain, this);
    // m_scene = new Scene(BasemapStyle::ArcGISImagery, this);

    // Create a scene view, and pass in the scene
    m_sceneView = new SceneGraphicsView(m_scene, this);

    // 假设您有一个场景（Scene）对象
    Esri::ArcGISRuntime::SpatialReference spatialReference = m_scene->spatialReference();

    // 输出空间参考的信息
    std::cout << "当前空间参考：" << spatialReference.wkText().toStdString()<<std::endl;

    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Sync_Viewpoint,this,[&](const Esri::ArcGISRuntime::Camera& camera){m_sceneView->setViewpointCameraAndWait(camera);});

#ifdef Demo

    // create a new elevation source from Terrain3D rest service
    ArcGISTiledElevationSource *elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

    auto surface = m_scene->baseSurface();
    surface->elevationSources()->append(elevationSource);// add the elevation source to the scene to display elevation
    // surface->setElevationExaggeration(3.28084);
    m_scene->setBaseSurface(surface);

    // create a camera
    constexpr double latitude = 33.961;
    constexpr double longitude = -118.808;
    constexpr double altitude = 2000;
    constexpr double heading = 0;
    constexpr double pitch = 75;
    constexpr double roll = 0;
    Camera camera{latitude, longitude, altitude, heading, pitch, roll};
    // set the viewpoint to the camera
    m_sceneView->setViewpointCameraAndWait(camera);
#else



    // create the MontereyElevation data path
    // data is downloaded automatically by the sample viewer app. Instructions to download
    // separately are specified in the readme.
    // const QString montereyRasterElevationPath = QString{"../../Resource/Copernicus_USA.tif"};//Copernicus
    // const QString montereyRasterElevationPath = QString{"../../Resource/srtm_36_02.img"};//地理空间数据云
    // const QString montereyRasterElevationPath = QString{"../../Resource/N34W119.hgt"};//30M SRTM
    const QString montereyRasterElevationPath = QString{"../../Resource/exportImage.tiff"};//ETOPO
    // const QString montereyRasterElevationPath = QString{"../../Resource/ALPSMLC30_N034W119_DSM.tif"};//AW3D30 DSM

    //Before attempting to add any layers, check that the file for the elevation source exists at all.
    const bool srcElevationFileExists = QFileInfo::exists(montereyRasterElevationPath);

    if(srcElevationFileExists)
    {
        //Create the elevation source from the local raster(s). RasterElevationSource can take multiple files as inputs, but in this case only takes one.
        RasterElevationSource* elevationSrc = new RasterElevationSource{QStringList{montereyRasterElevationPath}, this};

        // 添加更详细的加载状态监控
        connect(elevationSrc, &RasterElevationSource::doneLoading, this, [&](const Error& loadError)
                {
                    if(loadError.isEmpty()) {
                        qInfo() << "高程源加载成功";

                        // 获取数据范围并输出
                        // Esri::ArcGISRuntime::Envelope extent = elevationSrc->fullExtent();
                        // qInfo() << "数据范围:"
                        //         << "xmin: " << extent.xMin()
                        //         << "xmax: " << extent.xMax()
                        //         << "ymin: " << extent.yMin()
                        //         << "ymax: " << extent.yMax();

                        // 获取空间参考信息并输出
                        // Esri::ArcGISRuntime::SpatialReference spatialRef = elevationSrc->spatialReference();
                        // qInfo() << "空间参考:" << spatialRef.wkt();  // 使用 wkt() 获取空间参考的 WKT 表示形式
                    } else {
                        qWarning() << "加载错误:" << loadError.message();
                    }
                });


        // add the elevation source to the scene to display elevation
        m_scene->baseSurface()->elevationSources()->append(elevationSrc);
        // m_scene->baseSurface()->setElevationExaggeration(3.28084);
    }
    else
    {
        qWarning() << "Could not find file at : " << montereyRasterElevationPath << ". Elevation source not set.";
    }

    // // 直接定位到上海
    // Point cameraPoint(-121.6, 31.2, 1000, SpatialReference::wgs84());
    // Camera camera(cameraPoint, 0, 45, 0);
    // m_sceneView->setViewpointCameraAsync(camera);

    // create a camera
    constexpr double latitude = 33.961;
    constexpr double longitude = -118.808;
    constexpr double altitude = 2000;
    constexpr double heading = 0;
    constexpr double pitch = 75;
    constexpr double roll = 0;
    Camera camera{latitude, longitude, altitude, heading, pitch, roll};
    // set the viewpoint to the camera
    m_sceneView->setViewpointCameraAndWait(camera);

#endif

    // set the sceneView as the central widget
    setCentralWidget(m_sceneView);
}

DEM::~DEM() = default;
