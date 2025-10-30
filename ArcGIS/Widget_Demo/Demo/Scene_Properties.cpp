// Other headers
#include "Scene_Properties.hpp"

// C++ API headers
#include "ArcGISTiledElevationSource.h"
#include "AttributeListModel.h"
#include "Basemap.h"
#include "Camera.h"
#include "ElevationSourceListModel.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LayerSceneProperties.h"
#include "MapTypes.h"
#include "Point.h"
#include "RendererSceneProperties.h"
#include "Scene.h"
#include "SceneViewTypes.h"
#include "SimpleMarkerSceneSymbol.h"
#include "SimpleRenderer.h"
#include "SpatialReference.h"
#include "Surface.h"
#include "SymbolTypes.h"

#include <QUrl>
#include <QStandardPaths>
#include <QFileInfo>
#include <QFuture>
#include <QVBoxLayout>

#include "../Utils/Signal_Proxy.hpp"

using namespace Esri::ArcGISRuntime;

const QString HEADING("HEADING");
const QString PITCH("PITCH");
const QString ROLL("ROLL");

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

Scene_Properties::Scene_Properties(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a scene using the ArcGISTerrain BasemapStyle
    m_scene = new Scene(BasemapStyle::ArcGISTerrain, this);

    // Create a scene view, and pass in the scene
    m_sceneView = new SceneGraphicsView(m_scene, this);

    // create a new elevation source from Terrain3D REST service
    ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

    // add the elevation source to the scene to display elevation
    m_scene->baseSurface()->elevationSources()->append(elevationSource);

    m_graphicsOverlay = new GraphicsOverlay(this);
    m_graphicsOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Relative));

    // Create a SimpleRenderer and set expressions on its scene properties.
    // Then, set the renderer to the graphics overlay with GraphicsOverlay.setRenderer(renderer).
    SimpleRenderer* renderer3D = new SimpleRenderer(this);
    RendererSceneProperties renderProperties = renderer3D->sceneProperties();
    renderProperties.setHeadingExpression(QString("[%1]").arg(HEADING));
    renderProperties.setPitchExpression(QString("[%1]").arg(PITCH));
    renderProperties.setRollExpression(QString("[%1]").arg(ROLL));
    renderer3D->setSceneProperties(renderProperties);
    m_graphicsOverlay->setRenderer(renderer3D);

    m_sceneView->setArcGISScene(m_scene);

    // create a camera
    const double latitude = 32.09;
    const double longitude = -118.71;
    const double altitude = 100000.0;
    const double heading = 0.0;
    const double pitch = 45.0;
    const double roll = 0.0;
    const int coneDimension = 10000;

    Camera camera(latitude - 1.0, // place the camera arbitrarily south of the cone
                  longitude,
                  altitude * 2, // place the camera arbitrarily higher than the cone
                  heading,
                  pitch,
                  roll);

    // set the viewpoint
    m_sceneView->setViewpointCameraAndWait(camera);

    {// add the graphics overlay to the scene view
        m_sceneView->graphicsOverlays()->append(m_graphicsOverlay);

        // create a scene symbol based on the current type
        SimpleMarkerSceneSymbol* smss = new SimpleMarkerSceneSymbol(SimpleMarkerSceneSymbolStyle::Cube, QColor("red"), 200, 200, 200, SceneSymbolAnchorPosition::Center, this);
        smss->setWidth(coneDimension);
        smss->setDepth(coneDimension);
        smss->setHeight(coneDimension * 2);

        // create a graphic using the symbol above and a point location
        m_graphic = new Graphic(Point(longitude, latitude, altitude, m_sceneView->spatialReference()), smss, this);
        m_graphic->attributes()->insertAttribute(HEADING, 180);
        m_graphic->attributes()->insertAttribute(PITCH, 0);
        m_graphic->attributes()->insertAttribute(ROLL, 0);

        // add the graphic to the graphics overlay
        m_graphicsOverlay->graphics()->append(m_graphic);
    }

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_sceneView);
    setLayout(vBoxLayout);


    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Scene_Properties_Set_Heading,this,[&](double value){m_graphic->attributes()->replaceAttribute(HEADING, value);});
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Scene_Properties_Set_Picth,this,[&](double value){m_graphic->attributes()->replaceAttribute(PITCH, value);});
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Scene_Properties_Set_Roll,this,[&](double value){m_graphic->attributes()->replaceAttribute(ROLL, value);});
}

