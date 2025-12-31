// Other headers
#include "Animate_Symbol.hpp"

// C++ API headers
#include "ArcGISTiledElevationSource.h"
#include "AttributeListModel.h"
#include "Camera.h"
#include "DistanceCompositeSceneSymbol.h"
#include "ElevationSourceListModel.h"
#include "GlobeCameraController.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LayerSceneProperties.h"
#include "Map.h"
#include "MapTypes.h"
#include "ModelSceneSymbol.h"
#include "OrbitGeoElementCameraController.h"
#include "PointCollection.h"
#include "Polyline.h"
#include "PolylineBuilder.h"
#include "RendererSceneProperties.h"
#include "Scene.h"
#include "SceneViewTypes.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSceneSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "SpatialReference.h"
#include "Surface.h"
#include "SymbolTypes.h"
#include "Viewpoint.h"

#include <Camera.h>
#include <ArcGISSceneLayer.h>
#include <LayerListModel.h>
#include <Error.h>

#include <QUrl>
#include <QStandardPaths>
#include <QFileInfo>
#include <QFuture>
#include <QVBoxLayout>
#include <QStringListModel>
#include <QTimer>

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

static const QString HEADING = QStringLiteral("heading");
static const QString ROLL = QStringLiteral("roll");
static const QString PITCH = QStringLiteral("pitch");
static const QString ANGLE = QStringLiteral("angle");



void Animate_Symbol::updateTimerInterval()
{
    // 对应 QML 的 interval 计算公式
    double interval = 16.0 + 84.0 * (100.0 - m_animationSpeed) / 100.0;
    qDebug()<<"interval = "<<interval;
    timer->setInterval(static_cast<int>(interval));
}

void Animate_Symbol::_Signal_Bind()
{
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Mission_Selected,this,&Animate_Symbol::changeMission);
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Camera_Follow,this,&Animate_Symbol::setFollowing);
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Set_Zoom,this,&Animate_Symbol::setZoom);
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Set_Angle,this,&Animate_Symbol::setAngle);
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Set_Speed,this,&Animate_Symbol::setAnimationSpeed);
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Set_Frame,this,&Animate_Symbol::setMissionFrame);





    // === 等价于 QML Timer { ... } ===
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->setSingleShot(false);   // repeat: true

    // 定时器触发逻辑（对应 onTriggered: animate()）
    connect(timer, &QTimer::timeout, this, [&]()
    {
        animate();

        setMissionFrame(m_frame+1);
        if(m_frame>=missionSize())
            setMissionFrame(0);
    });

    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Play,this,[&](bool checked)
    {
        if (checked)
            timer->start();
        else
            timer->stop();
    });
}


Animate_Symbol::Animate_Symbol(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
    , m_dataPath(defaultDataPath() + "/ArcGIS/Runtime/Data/3D")
    , m_missionsModel(new QStringListModel({QStringLiteral("Grand Canyon"), QStringLiteral("Hawaii"), QStringLiteral("Pyrenees"), QStringLiteral("Snowdon")},this))
    , m_missionData(new MissionData)
{
    _Signal_Bind();

    // Create a scene using the ArcGISTerrain BasemapStyle
    m_scene = new Scene(BasemapStyle::ArcGISTerrain, this);

    // Create a scene view, and pass in the scene
    m_sceneView = new SceneGraphicsView(m_scene, this);

    // for use when not in following mode
    m_globeController = new GlobeCameraController(this);

    // create a new elevation source
    ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

    // add the elevation source to the scene to display elevation
    m_scene->baseSurface()->elevationSources()->append(elevationSource);

    // create a new graphics overlay and add it to the sceneview
    GraphicsOverlay* sceneOverlay = new GraphicsOverlay(this);
    sceneOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Absolute));
    m_sceneView->graphicsOverlays()->append(sceneOverlay);

    SimpleRenderer* renderer3D = new SimpleRenderer(this);
    RendererSceneProperties renderProperties = renderer3D->sceneProperties();
    renderProperties.setHeadingExpression(QString("[%1]").arg(HEADING));
    renderProperties.setPitchExpression(QString("[%1]").arg(PITCH));
    renderProperties.setRollExpression(QString("[%1]").arg(ROLL));
    renderer3D->setSceneProperties(renderProperties);
    sceneOverlay->setRenderer(renderer3D);

    // set up mini map
    Map* map = new Map(BasemapStyle::ArcGISImageryStandard, this);
    m_mapView=new MapGraphicsView(map,this);
    m_mapView->setAttributionTextVisible(false);
    m_mapView->setMap(map);

    // create a graphics overlay for the mini map
    GraphicsOverlay* mapOverlay = new GraphicsOverlay(this);
    m_mapView->graphicsOverlays()->append(mapOverlay);

    // set up route graphic
    createRoute2d(mapOverlay);

    // set up overlay 2D graphic
    createModel2d(mapOverlay);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_sceneView);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);


}

void Animate_Symbol::changeMission(const QString &missionNameStr)
{
    setMissionFrame(0);

    // read the mission data from the samples .csv files
    QString formattedname = missionNameStr;
    m_missionData->parse(m_dataPath + "/Missions/" + formattedname.remove(" ") + ".csv");

    // if the mission was loaded successfully, move to the start position
    if (missionReady() && m_routeGraphic)
    {
        // create a polyline representing the route for the mission
        PolylineBuilder* routeBldr = new PolylineBuilder(SpatialReference::wgs84(), this);
        for (int i = 0; i < missionSize(); ++i)
        {
            const MissionData::DataPoint& dp = m_missionData->dataAt(i);
            routeBldr->addPoint(dp.m_pos);
        }

        // set the polyline as a graphic on the mapView
        m_routeGraphic->setGeometry(routeBldr->toGeometry());

        m_mapView->setViewpointAndWait(Viewpoint(m_routeGraphic->geometry()));
        createGraphic3D();
    }

    // emit missionReadyChanged();
    // emit missionSizeChanged();
    Signal_Proxy::Instance().Animate_UI_Update_Frame_Size(missionSize());
}

void Animate_Symbol::animate()
{
    if (!m_missionData)
        return;

    if (missionFrame() < missionSize())
    {
        // get the data for this stage in the mission
        const MissionData::DataPoint& dp = m_missionData->dataAt(missionFrame());

        // move 3D graphic to the new position
        m_graphic3d->setGeometry(dp.m_pos);
        // update attribute expressions to immediately update rotation
        m_graphic3d->attributes()->replaceAttribute(HEADING, dp.m_heading);
        m_graphic3d->attributes()->replaceAttribute(PITCH, dp.m_pitch);
        m_graphic3d->attributes()->replaceAttribute(ROLL, dp.m_roll);

        // move 2D graphic to the new position
        m_graphic2d->setGeometry(dp.m_pos);
        m_symbol2d->setAngle(dp.m_heading);
    }

    // increment the frame count
    // emit nextFrameRequested();
}



void Animate_Symbol::createRoute2d(GraphicsOverlay *mapOverlay)
{
    // Create a 2d graphic of a solid red line to represen the route of the mission
    SimpleLineSymbol* routeSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::red, 1, this);
    m_routeGraphic = new Graphic(this);
    m_routeGraphic->setSymbol(routeSymbol);
    mapOverlay->graphics()->append(m_routeGraphic);
}

void Animate_Symbol::createModel2d(GraphicsOverlay *mapOverlay)
{
    if (m_symbol2d || m_graphic2d)
        return;

    // get the mission data for the frame
    const MissionData::DataPoint& dp = m_missionData->dataAt(missionFrame());

    // create a blue triangle symbol to represent the plane on the mini map
    m_symbol2d = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Triangle, Qt::blue, 10, this);
    m_symbol2d->setAngle(dp.m_heading);

    // create a graphic with the symbol
    m_graphic2d = new Graphic(dp.m_pos, m_symbol2d, this);

    mapOverlay->graphics()->append(m_graphic2d);
}

void Animate_Symbol::createGraphic3D()
{
    if (!missionReady())
        return;

    // create the ModelSceneSymbol to be animated in the 3d view
    if (!m_model3d)
        m_model3d = new ModelSceneSymbol(QUrl(m_dataPath + "/Bristol/Collada/Bristol.dae"), 10.0f, this);

    // get the mission data for the frame
    const MissionData::DataPoint& dp = m_missionData->dataAt(missionFrame());

    if (!m_graphic3d)
    {
        // create a graphic using the model symbol
        m_graphic3d = new Graphic(dp.m_pos, m_model3d, this);
        m_graphic3d->attributes()->insertAttribute(HEADING, dp.m_heading);
        m_graphic3d->attributes()->insertAttribute(PITCH, dp.m_pitch);
        m_graphic3d->attributes()->insertAttribute(ROLL, dp.m_roll);

        // add the graphic to the graphics overlay
        m_sceneView->graphicsOverlays()->at(0)->graphics()->append(m_graphic3d);

        // create the camera controller to follow the graphic
        m_followingController = new OrbitGeoElementCameraController(m_graphic3d, 500, this);
        m_sceneView->setCameraController(m_followingController);
    }
    else
    {
        // update existing graphic's geometry and attributes
        m_graphic3d->setGeometry(dp.m_pos);
        m_graphic3d->attributes()->replaceAttribute(HEADING, dp.m_heading);
        m_graphic3d->attributes()->replaceAttribute(PITCH, dp.m_pitch);
        m_graphic3d->attributes()->replaceAttribute(ROLL, dp.m_roll);
    }
}

