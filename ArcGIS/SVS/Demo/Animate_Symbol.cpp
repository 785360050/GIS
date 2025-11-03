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

void Animate_Symbol::_Syncronize_Viewpoint_To_Map_View()
{
    if (map_view && scene_view->isNavigating())
        map_view->setViewpointAsync(scene_view->currentViewpoint(ViewpointType::CenterAndScale), 0);
}

void Animate_Symbol::_Syncronize_Viewpoint_To_Scene_View()
{
    if (scene_view && map_view->isNavigating())
        scene_view->setViewpointAsync(map_view->currentViewpoint(ViewpointType::CenterAndScale), 0);
}



void Animate_Symbol::_Signal_Bind()
{
    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Animate_Mission_Selected, this, &Animate_Symbol::Mission_Change);
    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Animate_Camera_Follow, this, &Animate_Symbol::Set_Camera_Following);
    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Animate_Set_Zoom, this, [this](double zoom_distance){camera_controller_OrbitGeoElement->setCameraDistance(zoom_distance);});
    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Animate_Set_Angle, this, [this](double angle){camera_controller_OrbitGeoElement->setCameraPitchOffset(angle);});
    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Animate_Set_Speed, this, &Animate_Symbol::Update_Animate_Speed);
    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Animate_Set_Frame, this, &Animate_Symbol::Set_Mission_Frame);

    connect(scene_view, &SceneGraphicsView::mouseClicked, this, &Animate_Symbol::displayElevationOnClick);


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


    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Enable_Viewpoint_Syncronize, this, &Animate_Symbol::Enable_Viewpoint_Syncronize);

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
    // graphic_2d = new Graphic(dp.pos, symbol_2d, this);
    graphic_2d = new Graphic;
    graphic_2d->setSymbol(symbol_2d);
    graphic_2d->setParent(this);
    graphic_2d->setGeometry(dp.pos);

    mapOverlay->graphics()->append(graphic_2d);
}

void Animate_Symbol::_Initialize_Scene_View()
{
    if (!mission_data.ready)
        return;

    // create the ModelSceneSymbol to be animated in the 3d view
    if (!model_3d)
    {
        SimpleMarkerSymbol* sms = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("red"), 10.0f, this);
        //! [create simple marker scene symbol]
        const SimpleMarkerSceneSymbolStyle style = SimpleMarkerSceneSymbolStyle::Cone;
        const QColor color("red");
        constexpr double height = 75.0;
        constexpr double width = 75.0;
        constexpr double depth = 75.0;
        const SceneSymbolAnchorPosition anchorPosition = SceneSymbolAnchorPosition::Bottom;
        SimpleMarkerSceneSymbol* smss = new SimpleMarkerSceneSymbol(style, color, height, width, depth, anchorPosition, this);
        //! [create simple marker scene symbol]

        //! [create distance symbol ranges with each symbol type and a distance range(meters)]
        DistanceSymbolRange* dsrModel = new DistanceSymbolRange(new ModelSceneSymbol(QUrl(DATA_PATH + "/Bristol/Collada/Bristol.dae"), 10.0f, this), 0, 999, this); // ModelSceneSymbol
        DistanceSymbolRange* dsrCone = new DistanceSymbolRange(smss, 1000, 2999, this); // SimpleMarkerSceneSymbol
        DistanceSymbolRange* dsrCircle = new DistanceSymbolRange(sms, 3000, 0, this); // SimpleMarkerSymbol

        DistanceCompositeSceneSymbol* compositeSceneSymbol = new DistanceCompositeSceneSymbol(this);

        compositeSceneSymbol->ranges()->append(dsrModel);
        compositeSceneSymbol->ranges()->append(dsrCone);
        compositeSceneSymbol->ranges()->append(dsrCircle);
        //! [create distance symbol ranges with each symbol type and a distance range(meters)]
        model_3d=compositeSceneSymbol;
    }

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

    {// Elevation Point in scene
        m_elevationMarker=new Graphic(Geometry(), new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("red"), 12, this), this);
        // Set the marker to be invisible initially, will be flaggd visible when user interacts with scene for the first time, to visualise clicked position
        m_elevationMarker->setVisible(false);

        m_graphicsOverlay=new GraphicsOverlay(this);
        // Add the marker to the graphics overlay so it will be displayed. Graphics overlay is attached to the sceneView in ::setSceneView()
        m_graphicsOverlay->graphics()->append(m_elevationMarker);
        // Append the graphics overlays to the sceneview, so we can visualise elevation on click
        scene_view->graphicsOverlays()->append(m_graphicsOverlay);
    }

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(scene_view);
    vBoxLayout->addWidget(map_view);
    setLayout(vBoxLayout);

    _Signal_Bind();


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
    _Update_Map(mission_data.point[frame_index]);
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

void Animate_Symbol::Enable_Viewpoint_Syncronize(bool enable)
{
    if(enable)
    {
        connect(scene_view, &SceneGraphicsView::viewpointChanged, this,&Animate_Symbol::_Syncronize_Viewpoint_To_Map_View);
        connect(map_view, &MapGraphicsView::viewpointChanged, this,&Animate_Symbol::_Syncronize_Viewpoint_To_Scene_View);
    }
    else
    {
        disconnect(scene_view, &SceneGraphicsView::viewpointChanged, this,&Animate_Symbol::_Syncronize_Viewpoint_To_Map_View);
        disconnect(map_view, &MapGraphicsView::viewpointChanged, this,&Animate_Symbol::_Syncronize_Viewpoint_To_Scene_View);

    }
}




void Animate_Symbol::_Update_Scene(const Mission_Data::Point &data_point)
{
    // move 3D graphic to the new position
    graphic_3d->setGeometry(data_point.pos);
    // update attribute expressions to immediately update rotation
    graphic_3d->attributes()->replaceAttribute(HEADING, data_point.heading);
    graphic_3d->attributes()->replaceAttribute(PITCH, data_point.pitch);
    graphic_3d->attributes()->replaceAttribute(ROLL, data_point.roll);
}
void Animate_Symbol::_Update_Map(const Mission_Data::Point &data_point)
{
    // move 2D graphic to the new position
    graphic_2d->setGeometry(data_point.pos);
    symbol_2d->setAngle(data_point.heading);
}
void Animate_Symbol::Frame_Update()
{

    if(frame_index<0 or frame_index>=mission_data.point.size())
        return;

    // get the data for this stage in the mission
    const Mission_Data::Point &data_point = mission_data.point[frame_index];

    // // move 3D graphic to the new position
    // graphic_3d->setGeometry(dp.pos);
    // // update attribute expressions to immediately update rotation
    // graphic_3d->attributes()->replaceAttribute(HEADING, dp.heading);
    // graphic_3d->attributes()->replaceAttribute(PITCH, dp.pitch);
    // graphic_3d->attributes()->replaceAttribute(ROLL, dp.roll);

    // // move 2D graphic to the new position
    // graphic_2d->setGeometry(dp.pos);
    // symbol_2d->setAngle(dp.heading);

    _Update_Scene(data_point);
    _Update_Map(data_point);
}




void Animate_Symbol::displayElevationOnClick(QMouseEvent &mouseEvent)
{
    // Convert clicked screen position to position on the map surface.
    const Esri::ArcGISRuntime::Point baseSurfacePos = scene_view->screenToBaseSurface(mouseEvent.position().x(), mouseEvent.position().y());

    m_elevationQueryFuture = scene->baseSurface()->elevationAsync(baseSurfacePos);
    m_elevationQueryFuture.then(this,
                                [this, baseSurfacePos](double elevation)
                                {
                                    // Place the elevation marker circle at the clicked position
                                    m_elevationMarker->setGeometry(baseSurfacePos);
                                    m_elevationMarker->setVisible(true);

                                    Signal_Proxy::Instance().Point_Elevation_Show(elevation);
                                });

}



