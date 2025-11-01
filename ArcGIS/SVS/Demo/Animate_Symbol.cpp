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

static QString DATA_PATH;



void Animate_Symbol::_Signal_Bind()
{
    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Animate_Mission_Selected, this, &Animate_Symbol::Mission_Change);
    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Animate_Camera_Follow, this, &Animate_Symbol::Set_Camera_Following);
    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Animate_Set_Zoom, this, [this](double zoom_distance){camera_controller_OrbitGeoElement->setCameraDistance(zoom_distance);});
    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Animate_Set_Angle, this, [this](double angle){camera_controller_OrbitGeoElement->setCameraPitchOffset(angle);});
    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Animate_Set_Speed, this, &Animate_Symbol::Update_Animate_Speed);
    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Animate_Set_Frame, this, &Animate_Symbol::Set_Mission_Frame);

    // === 等价于 QML Timer { ... } ===
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->setSingleShot(false); // repeat: true
    timer->setInterval(16);
    timer->stop();

    // 定时器触发逻辑（对应 onTriggered: animate()）
    connect(timer, &QTimer::timeout, this, [&]()
    {
        Frame_Update();

        // qDebug()<<frame_index;
        Set_Mission_Frame(frame_index+1);
        if(frame_index+1>=mission_data.point.size())
            Set_Mission_Frame(0);

        emit Signal_Proxy::Instance().Animate_UI_Update_Frame_Index(frame_index);// reflect to ui
    });

    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Animate_Play, this, [&](bool checked)
    {
        if (checked)
            timer->start();
        else
            timer->stop();
    });
}

void Animate_Symbol::_Initialize_Map_View(GraphicsOverlay *mapOverlay)
{
    /// Route2d
    // Create a 2d graphic of a solid red line to represen the route of the mission
    graphic_route = new Graphic(this);

    SimpleLineSymbol *routeSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::red, 1, this);
    graphic_route->setSymbol(routeSymbol);

    mapOverlay->graphics()->append(graphic_route);


    /// Model2d
    if (symbol_2d || graphic_2d)
        return;

    // get the mission data for the frame
    Mission_Data::Point dp{};
    if(!mission_data.point.empty())
        dp = mission_data.point[frame_index];

    // create a blue triangle symbol to represent the plane on the mini map
    symbol_2d = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Triangle, Qt::blue, 10, this);
    symbol_2d->setAngle(dp.heading);

    // create a graphic with the symbol
    graphic_2d = new Graphic(dp.pos, symbol_2d, this);

    mapOverlay->graphics()->append(graphic_2d);
}

void Animate_Symbol::_Initialize_Scene_View()
{
    if (!mission_data.ready)
        return;

    // create the ModelSceneSymbol to be animated in the 3d view
    if (!model_3d)
        model_3d = new ModelSceneSymbol(QUrl(DATA_PATH + "/Bristol/Collada/Bristol.dae"), 10.0f, this);

    // get the mission data for the frame
    Mission_Data::Point dp{};
    if(!mission_data.point.empty())
        dp = mission_data.point[frame_index];

    if (!graphic_3d)
    {
        // create a graphic using the model symbol
        graphic_3d = new Graphic(dp.pos, model_3d, this);
        graphic_3d->attributes()->insertAttribute(HEADING, dp.heading);
        graphic_3d->attributes()->insertAttribute(PITCH, dp.pitch);
        graphic_3d->attributes()->insertAttribute(ROLL, dp.roll);

        // add the graphic to the graphics overlay
        scene_view->graphicsOverlays()->at(0)->graphics()->append(graphic_3d);

        // create the camera controller to follow the graphic
        camera_controller_OrbitGeoElement = new OrbitGeoElementCameraController(graphic_3d, 500, this);
        scene_view->setCameraController(camera_controller_OrbitGeoElement);
    }
    else
    {
        // update existing graphic's geometry and attributes
        graphic_3d->setGeometry(dp.pos);
        graphic_3d->attributes()->replaceAttribute(HEADING, dp.heading);
        graphic_3d->attributes()->replaceAttribute(PITCH, dp.pitch);
        graphic_3d->attributes()->replaceAttribute(ROLL, dp.roll);
    }
}

Animate_Symbol::Animate_Symbol(QWidget *parent /*=nullptr*/)
    : QWidget(parent)
{
    DATA_PATH = defaultDataPath() + "/ArcGIS/Runtime/Data/3D";

    _Signal_Bind();

    // Create a scene using the ArcGISTerrain BasemapStyle
    scene = new Scene(BasemapStyle::ArcGISTerrain, this);

    // Create a scene view, and pass in the scene
    scene_view = new SceneGraphicsView(scene, this);

    // for use when not in following mode
    camera_controller_globe = new GlobeCameraController(this);

    // create a new elevation source
    ArcGISTiledElevationSource *elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

    // add the elevation source to the scene to display elevation
    scene->baseSurface()->elevationSources()->append(elevationSource);

    // create a new graphics overlay and add it to the sceneview
    GraphicsOverlay *sceneOverlay = new GraphicsOverlay(this);
    sceneOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Absolute));
    scene_view->graphicsOverlays()->append(sceneOverlay);

    SimpleRenderer *renderer3D = new SimpleRenderer(this);
    RendererSceneProperties renderProperties = renderer3D->sceneProperties();
    renderProperties.setHeadingExpression(QString("[%1]").arg(HEADING));
    renderProperties.setPitchExpression(QString("[%1]").arg(PITCH));
    renderProperties.setRollExpression(QString("[%1]").arg(ROLL));
    renderer3D->setSceneProperties(renderProperties);
    sceneOverlay->setRenderer(renderer3D);

    // set up mini map
    Map *map = new Map(BasemapStyle::ArcGISImageryStandard, this);
    map_view = new MapGraphicsView(map, this);
    map_view->setAttributionTextVisible(false);
    map_view->setMap(map);

    // create a graphics overlay for the mini map
    GraphicsOverlay *mapOverlay = new GraphicsOverlay(this);
    map_view->graphicsOverlays()->append(mapOverlay);

    // set up overlay 2D graphic
    _Initialize_Map_View(mapOverlay);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(scene_view);
    vBoxLayout->addWidget(map_view);
    setLayout(vBoxLayout);
}

void Animate_Symbol::Mission_Change(const QString &missionNameStr)
{
    Set_Mission_Frame(0);

    // read the mission data from the samples .csv files
    QString formattedname = missionNameStr;
    mission_data.Load_File(DATA_PATH + "/Missions/" + formattedname.remove(" ") + ".csv");

    // if the mission was loaded successfully, move to the start position
    if (mission_data.ready && graphic_route)
    {
        // create a polyline representing the route for the mission
        PolylineBuilder *routeBldr = new PolylineBuilder(SpatialReference::wgs84(), this);
        for (int i = 0; i < mission_data.point.size(); ++i)
        {
            const Mission_Data::Point &dp = mission_data.point[i];
            routeBldr->addPoint(dp.pos);
        }

        // set the polyline as a graphic on the mapView
        graphic_route->setGeometry(routeBldr->toGeometry());

        map_view->setViewpointAndWait(Viewpoint(graphic_route->geometry()));
        _Initialize_Scene_View();
    }

    // emit missionReadyChanged();
    // emit missionSizeChanged();
    Signal_Proxy::Instance().Animate_UI_Update_Frame_Size(mission_data.point.size());
}

void Animate_Symbol::Update_Animate_Speed(double value)
{
    animation_speed = std::clamp(value, 0.0, 100.0);
    // 对应 QML 的 interval 计算公式
    double interval = 16.0 + 84.0 * (100.0 - animation_speed) / 100.0;
    qDebug() << "interval = " << interval;
    timer->setInterval(static_cast<int>(interval));
}

void Animate_Symbol::Frame_Update()
{

    if(frame_index<0 or frame_index>=mission_data.point.size())
        return;

    // get the data for this stage in the mission
    const Mission_Data::Point &dp = mission_data.point[frame_index];

    // move 3D graphic to the new position
    graphic_3d->setGeometry(dp.pos);
    // update attribute expressions to immediately update rotation
    graphic_3d->attributes()->replaceAttribute(HEADING, dp.heading);
    graphic_3d->attributes()->replaceAttribute(PITCH, dp.pitch);
    graphic_3d->attributes()->replaceAttribute(ROLL, dp.roll);

    // move 2D graphic to the new position
    graphic_2d->setGeometry(dp.pos);
    symbol_2d->setAngle(dp.heading);
}




