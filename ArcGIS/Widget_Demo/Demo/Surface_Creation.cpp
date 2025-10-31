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
#include "Surface_Creation.hpp"

// C++ API headers
#include "ArcGISTiledElevationSource.h"
#include "ElevationSourceListModel.h"
#include "RasterElevationSource.h"
#include "MapTypes.h"
#include "Scene.h"
#include "SceneGraphicsView.h"
#include "Surface.h"

#include <Camera.h>
#include <Error.h>

#include <QUrl>
#include <QStandardPaths>
#include <QFileInfo>
#include <QVBoxLayout>

using namespace Esri::ArcGISRuntime;

// #define Local_File_Tile
#define Local_File_Raster

static QString defaultDataPath()
{
    QString dataPath;

#ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

    return dataPath;
}

Surface_Creation::Surface_Creation(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a scene using the ArcGISTerrain BasemapStyle
    m_scene = new Scene(BasemapStyle::ArcGISTerrain, this);

    // Create a scene view, and pass in the scene
    m_sceneView = new SceneGraphicsView(m_scene, this);


#ifdef Local_File_Tile
    /// ============================================================================================================
    /// 		此处离线文件只是一个平面surface，如果要3D高程显示，需要在该surface之下添加一层高程surface
    /// ============================================================================================================
    // create the MontereyElevation data path
    // data is downloaded automatically by the sample viewer app. Instructions to download separately are specified in the readme.
    const QString montereyTileElevationPath = QString{defaultDataPath() + "/ArcGIS/Runtime/Data/tpkx/MontereyElevation.tpkx"};

    // Before attempting to add any layers, check that the file for the elevation source exists at all.
    const bool srcElevationFileExists = QFileInfo::exists(montereyTileElevationPath);

    if(srcElevationFileExists)
    {
        // Create the elevation source from the local raster(s). RasterElevationSource can take multiple files as inputs, but in this case only takes one.
        ArcGISTiledElevationSource* elevationSrc = new ArcGISTiledElevationSource{montereyTileElevationPath, this};

        // When the elevation source is finished loading, call the elevationSrcFinishedLoading callback, so we can tell if it loaded succesfully.
        connect(elevationSrc, &ArcGISTiledElevationSource::doneLoading, this, [&](const Error& loadError)
                {
                    if(loadError.isEmpty())
                    {
                        // Succesful load
                        qDebug() << "Loaded tile elevation source succesfully";
                    }
                    else {
                        // Log failure to load
                        qDebug() << "Error loading elevation source : " << loadError.message();
                    }
                });

        // add the elevation source to the scene to display elevation
        m_scene->baseSurface()->elevationSources()->append(elevationSrc);
    }
    else
    {
        qDebug() << "Could not find file at : " << montereyTileElevationPath << ". Elevation source not set.";
    }
#elif defined(Local_File_Raster)
    // create the MontereyElevation data path
    // data is downloaded automatically by the sample viewer app. Instructions to download
    // separately are specified in the readme.
    const QString montereyRasterElevationPath = QString{defaultDataPath() + "/ArcGIS/Runtime/Data/raster/MontereyElevation.dt2"};

    //Before attempting to add any layers, check that the file for the elevation source exists at all.
    const bool srcElevationFileExists = QFileInfo::exists(montereyRasterElevationPath);

    if(srcElevationFileExists)
    {
        //Create the elevation source from the local raster(s). RasterElevationSource can take multiple files as inputs, but in this case only takes one.
        RasterElevationSource* elevationSrc = new RasterElevationSource{QStringList{montereyRasterElevationPath}, this};

        //When the elevation source is finished loading, call the elevationSrcFinishedLoading callback, so we can tell if it loaded succesfully.
        connect(elevationSrc, &RasterElevationSource::doneLoading, this, [&](const Error& loadError)
                {
                    if(loadError.isEmpty())
                    {
                        //Succesful load
                        qInfo() << "Loaded raster elevation source succesfully";
                    }
                    else {
                        //Log failure to load
                        qWarning() << "Error loading elevation source : " << loadError.message();
                    }
                });

        // add the elevation source to the scene to display elevation
        m_scene->baseSurface()->elevationSources()->append(elevationSrc);
    }
    else
    {
        qWarning() << "Could not find file at : " << montereyRasterElevationPath << ". Elevation source not set.";
    }

#else // Online Basemap
    // create a new elevation source from Terrain3D rest service
    ArcGISTiledElevationSource *elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

    auto surface = m_scene->baseSurface();
    surface->elevationSources()->append(elevationSource);// add the elevation source to the scene to display elevation
    surface->setElevationExaggeration(2.5);
    m_scene->setBaseSurface(surface);

#endif

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

    // set the sceneView as the central widget
    // setCentralWidget(m_sceneView);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_sceneView);
    setLayout(vBoxLayout);
}

