// Other headers
#include "Imagery_Surface.hpp"

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

Imagery_Surface::Imagery_Surface(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a scene using the ArcGISTerrain BasemapStyle
    m_scene = new Scene(BasemapStyle::ArcGISImagery, this);

    // Create a scene view, and pass in the scene
    m_sceneView = new SceneGraphicsView(m_scene, this);

    // create a new elevation source from Terrain3D rest service
    ArcGISTiledElevationSource *elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

    auto surface = m_scene->baseSurface();
    surface->elevationSources()->append(elevationSource);// add the elevation source to the scene to display elevation
    surface->setElevationExaggeration(2.5);
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

    // set the sceneView as the central widget
    // setCentralWidget(m_sceneView);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_sceneView);
    setLayout(vBoxLayout);
}

Imagery_Surface::~Imagery_Surface() = default;
