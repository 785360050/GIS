// Other headers
#include "Tile_Layer_3D.hpp"

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

Tile_Layer_3D::Tile_Layer_3D(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    this->setWindowTitle("Tile_Layer_3D");

    // Create a scene using the ArcGISTerrain BasemapStyle
    m_scene = new Scene(BasemapStyle::ArcGISTerrain, this);

    // Create a scene view, and pass in the scene
    m_sceneView = new SceneGraphicsView(m_scene, this);

    // create a new elevation source from Terrain3D REST service
    ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

    // add the elevation source to the scene to display elevation
    m_scene->baseSurface()->elevationSources()->append(elevationSource);

    add3DTilesLayer();

    // add a camera
    constexpr double latitude = 48.8418;
    constexpr double longitude = 9.1536;
    constexpr double altitude = 1325.0;
    constexpr double heading = 48.3497;
    constexpr double pitch = 57.8414;
    constexpr double roll = 0.0;
    const Camera sceneCamera(latitude, longitude, altitude, heading, pitch, roll);
    m_sceneView->setViewpointCameraAndWait(sceneCamera);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_sceneView);
    setLayout(vBoxLayout);
}

void Tile_Layer_3D::add3DTilesLayer()
{
    const QUrl modelPath = QUrl("https://tiles.arcgis.com/tiles/ZQgQTuoyBrtmoGdP/arcgis/rest/services/Stuttgart/3DTilesServer/tileset.json");
    m_ogc3dTilesLayer = new Ogc3dTilesLayer(modelPath, this);
    m_scene->operationalLayers()->append(m_ogc3dTilesLayer);
}

