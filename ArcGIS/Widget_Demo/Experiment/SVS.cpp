// Other headers
#include "SVS.hpp"

// C++ API headers
#include "ArcGISTiledElevationSource.h"
#include "ElevationSourceListModel.h"
#include "RasterElevationSource.h"
#include "MapTypes.h"
#include "SpatialReference.h"
#include "Scene.h"
#include "Viewpoint.h"
#include "Point.h"
#include "SceneGraphicsView.h"
#include "Surface.h"

#include <Camera.h>
#include <ArcGISSceneLayer.h>
#include <LayerListModel.h>
#include <Error.h>

#include <QUrl>
#include <QStandardPaths>
#include <QFileInfo>
#include <QFuture>
#include <QVBoxLayout>

using namespace Esri::ArcGISRuntime;

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

SVS::SVS(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // scene=new Scene(BasemapStyle::ArcGISImageryStandard, this);
    // scene_view = new SceneGraphicsView(scene, this);


    // map=new Map(BasemapStyle::ArcGISImageryStandard, this);
    // map_view=new MapGraphicsView(map,this);

    scene_view->setArcGISScene(scene);

    connect(scene_view, &SceneGraphicsView::viewpointChanged, this,[this]()
    {
        if (map_view && scene_view->isNavigating())
            map_view->setViewpointAsync(scene_view->currentViewpoint(ViewpointType::CenterAndScale), 0);
    });

    map_view->setMap(map);
    map_view->setRotationByPinchingEnabled(true);

    connect(map_view, &MapGraphicsView::viewpointChanged, this,[this]()
    {
        if (scene_view && map_view->isNavigating())
            scene_view->setViewpointAsync(map_view->currentViewpoint(ViewpointType::CenterAndScale), 0);
    });


    // create a new elevation source from Terrain3D rest service
    ArcGISTiledElevationSource *elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

    auto surface = scene->baseSurface();
    surface->elevationSources()->append(elevationSource);// add the elevation source to the scene to display elevation
    surface->setElevationExaggeration(2.5);
    scene->setBaseSurface(surface);

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
        scene->baseSurface()->elevationSources()->append(elevationSrc);
    }
    else
    {
        qWarning() << "Could not find file at : " << montereyRasterElevationPath << ". Elevation source not set.";
    }


    // create a camera
    constexpr double latitude = 33.961;
    constexpr double longitude = -118.808;
    constexpr double altitude = 2000;
    constexpr double heading = 0;
    constexpr double pitch = 75;
    constexpr double roll = 0;
    Camera camera{latitude, longitude, altitude, heading, pitch, roll};
    // set the viewpoint to the camera
    scene_view->setViewpointCameraAndWait(camera);
    // Camera camera = scene_view->currentViewpointCamera();
    map_view->setViewpointAndWait(scene_view->currentViewpoint(ViewpointType::BoundingGeometry));

    // Set up the UI
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(scene_view);
    layout->addWidget(map_view);
    setLayout(layout);
}

