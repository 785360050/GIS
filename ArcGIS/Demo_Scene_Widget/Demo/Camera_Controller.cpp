// Other headers
#include "Camera_Controller.hpp"

// C++ API headers
#include "ArcGISTiledElevationSource.h"
#include "ElevationSourceListModel.h"
#include "RasterElevationSource.h"
#include "MapTypes.h"
#include "SpatialReference.h"
#include "Scene.h"
#include "Viewpoint.h"
#include "ModelSceneSymbol.h"
#include "Point.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "SceneGraphicsView.h"
#include "Surface.h"

#include <Camera.h>
#include <ArcGISSceneLayer.h>
#include <LayerListModel.h>
#include <Error.h>

#include "GraphicListModel.h"
#include "LayerSceneProperties.h"
#include "SceneViewTypes.h"

#include <QUrl>
#include <QStandardPaths>
#include <QFileInfo>
#include <QFuture>
#include <QVBoxLayout>

#include "../Utils/Signal_Proxy.hpp"

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

static const double longitude = -109.929589;
static const double latitude = 38.43500;
static const double distance = 5000;

Camera_Controller::Camera_Controller(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
    , m_orbitGlobe(new GlobeCameraController(this))
    , m_orbitLocation(new OrbitLocationCameraController(Point(longitude, latitude, 0.0, SpatialReference::wgs84()), 100, this))
    , m_scene(new Scene(BasemapStyle::ArcGISImageryStandard, this))
{
    // Create a scene using the ArcGISTerrain BasemapStyle
    m_scene = new Scene(BasemapStyle::ArcGISImageryStandard, this);

    // Create a scene view, and pass in the scene
    m_sceneView = new SceneGraphicsView(m_scene, this);

    {
        // create a new elevation source from Terrain3D REST service
        ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

        // add the elevation source to the scene to display elevation
        m_scene->baseSurface()->elevationSources()->append(elevationSource);

        m_orbitLocation->setCameraDistance(distance);

        // get the data path
        QUrl dataUrl(defaultDataPath() + "/ArcGIS/Runtime/Data/3D/Bristol/Collada/Bristol.dae");
        ModelSceneSymbol* planeModel = new ModelSceneSymbol(dataUrl, 10.0f, this);

        Esri::ArcGISRuntime::Graphic* graphic = new Graphic(Point(longitude, latitude, distance / 2), planeModel, this);

        m_overlay = new GraphicsOverlay(this);

        m_overlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Relative));
        m_overlay->graphics()->append(graphic);
        m_orbitGeoElement = new OrbitGeoElementCameraController(graphic, distance / 2, this);
    }


    // set an initial viewpoint
    m_sceneView->setViewpointCameraAsync(Camera(latitude, longitude, distance, 0.0, 0.0, 0.0));
    m_sceneView->setArcGISScene(m_scene);
    m_sceneView->graphicsOverlays()->append(m_overlay);

    // set the scene to the scene view
    m_sceneView->setArcGISScene(m_scene);

    m_sceneView->setCameraController(m_orbitGlobe);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_sceneView);
    setLayout(vBoxLayout);


    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Camera_GlobeCameraController,this,[&](){m_sceneView->setCameraController(m_orbitGlobe);});
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Camera_OrbitGeoElementCameraController,this,[&](){m_sceneView->setCameraController(m_orbitGeoElement);});
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Camera_OrbitLocationCameraControlle,this,[&](){m_sceneView->setCameraController(m_orbitLocation);});
}

