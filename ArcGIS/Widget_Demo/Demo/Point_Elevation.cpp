// Other headers
#include "Point_Elevation.hpp"

// C++ API headers
#include "ArcGISTiledElevationSource.h"
#include "Camera.h"
#include "ElevationSourceListModel.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "MapTypes.h"
#include "Point.h"
#include "Scene.h"
#include "Graphic.h"
#include "SimpleMarkerSymbol.h"
#include "Surface.h"
#include "SymbolTypes.h"

#include <Camera.h>
#include <ArcGISSceneLayer.h>
#include <LayerListModel.h>
#include <Error.h>

#include <QMouseEvent>
#include <QUrl>
#include <QStandardPaths>
#include <QFileInfo>
#include <QFuture>
#include <QVBoxLayout>

#include "../Utils/Signal_Proxy.hpp"

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

Point_Elevation::Point_Elevation(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a scene using the ArcGISTerrain BasemapStyle
    m_scene = new Scene(BasemapStyle::ArcGISImageryStandard, this);

    // Create a scene view, and pass in the scene
    m_sceneView = new SceneGraphicsView(m_scene, this);

    m_graphicsOverlay=new GraphicsOverlay(this);
    m_elevationMarker=new Graphic(Geometry(), new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("red"), 12, this), this);

    // create a new elevation source from Terrain3D REST service
    ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

    // add the elevation source to the scene to display elevation
    m_scene->baseSurface()->elevationSources()->append(elevationSource);

    // Set the marker to be invisible initially, will be flaggd visible when user interacts with scene for the first time, to visualise clicked position
    m_elevationMarker->setVisible(false);

    // Add the marker to the graphics overlay so it will be displayed. Graphics overlay is attached to the sceneView in ::setSceneView()
    m_graphicsOverlay->graphics()->append(m_elevationMarker);

    m_sceneView->setArcGISScene(m_scene);

    // Create a camera, looking at the Himalayan mountain range.
    constexpr double latitude = 28.4;
    constexpr double longitude = 83.9;
    constexpr double altitude = 10000.0;
    constexpr double heading = 10.0;
    constexpr double pitch = 80.0;
    constexpr double roll = 0.0;
    Camera camera(latitude, longitude, altitude, heading, pitch, roll);

    // Set the sceneview to use above camera, waits for load so scene is immediately displayed in appropriate place.
    m_sceneView->setViewpointCameraAndWait(camera);

    // Append the graphics overlays to the sceneview, so we can visualise elevation on click
    m_sceneView->graphicsOverlays()->append(m_graphicsOverlay);

    // Hook up clicks into the 3d scene to below behaviour that displays marker & elevation value.
    connect(this->m_sceneView, &SceneGraphicsView::mouseClicked, this, &Point_Elevation::displayElevationOnClick);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_sceneView);
    setLayout(vBoxLayout);
}

void Point_Elevation::displayElevationOnClick(QMouseEvent &mouseEvent)
{
    // Convert clicked screen position to position on the map surface.
    const Point baseSurfacePos = m_sceneView->screenToBaseSurface(mouseEvent.position().x(), mouseEvent.position().y());

    m_elevationQueryFuture = m_scene->baseSurface()->elevationAsync(baseSurfacePos);
    m_elevationQueryFuture.then(this,
                                [this, baseSurfacePos](double elevation)
                                {
                                    // Place the elevation marker circle at the clicked position
                                    m_elevationMarker->setGeometry(baseSurfacePos);
                                    m_elevationMarker->setVisible(true);

                                    // Assign the elevation value. UI is bound to this value, so it updates to display new elevation.
                                    m_elevation = elevation;

                                    Signal_Proxy::Instance().Point_Elevation_Show(m_elevation);
                                });

}

