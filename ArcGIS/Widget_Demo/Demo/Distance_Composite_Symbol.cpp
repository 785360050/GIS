// Other headers
#include "Distance_Composite_Symbol.hpp"

// C++ API headers
#include "ArcGISTiledElevationSource.h"
#include "Basemap.h"
#include "DistanceCompositeSceneSymbol.h"
#include "ElevationSourceListModel.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LayerSceneProperties.h"
#include "MapTypes.h"
#include "ModelSceneSymbol.h"
#include "OrbitGeoElementCameraController.h"
#include "Point.h"
#include "Scene.h"
#include "SceneViewTypes.h"
#include "SimpleMarkerSceneSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SpatialReference.h"
#include "Surface.h"
#include "SymbolTypes.h"

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

Distance_Composite_Symbol::Distance_Composite_Symbol(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a scene using the ArcGISTerrain BasemapStyle
    m_scene = new Scene(BasemapStyle::ArcGISImageryStandard, this);

    // Create a scene view, and pass in the scene
    m_sceneView = new SceneGraphicsView(m_scene, this);

    // get the data path
    const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/3D/Bristol/Collada/Bristol.dae";

    // set scene on the scene view
    m_sceneView->setArcGISScene(m_scene);

    // create a new elevation source
    ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);
    // add the elevation source to the scene to display elevation
    m_scene->baseSurface()->elevationSources()->append(elevationSource);

    // create a camera
    const Point point(-2.708471, 56.096575, 5000, m_sceneView->spatialReference());

    // create a new graphics overlay and add it to the sceneview
    GraphicsOverlay* graphicsOverlay = new GraphicsOverlay(this);
    graphicsOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Relative));
    m_sceneView->graphicsOverlays()->append(graphicsOverlay);

    //! [create model scene symbol]
    constexpr float scale = 5.0f;
    ModelSceneSymbol* mms = new ModelSceneSymbol(QUrl(dataPath), scale, this);
    mms->setHeading(180);
    //! [create model scene symbol]

    connect(mms, &ModelSceneSymbol::loadStatusChanged, this, [mms, point, graphicsOverlay, this](){
        if (mms->loadStatus() == LoadStatus::Loaded)
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
            DistanceSymbolRange* dsrModel = new DistanceSymbolRange(mms, 0, 999, this); // ModelSceneSymbol
            DistanceSymbolRange* dsrCone = new DistanceSymbolRange(smss, 1000, 2999, this); // SimpleMarkerSceneSymbol
            DistanceSymbolRange* dsrCircle = new DistanceSymbolRange(sms, 3000, 0, this); // SimpleMarkerSymbol

            DistanceCompositeSceneSymbol* compositeSceneSymbol = new DistanceCompositeSceneSymbol(this);

            compositeSceneSymbol->ranges()->append(dsrModel);
            compositeSceneSymbol->ranges()->append(dsrCone);
            compositeSceneSymbol->ranges()->append(dsrCircle);
            //! [create distance symbol ranges with each symbol type and a distance range(meters)]

            // create a graphic using the composite symbol
            Graphic* graphic = new Graphic(point, compositeSceneSymbol, this);

            // add the graphic to the graphics overlay
            graphicsOverlay->graphics()->append(graphic);

            // add an orbit camera controller to lock the camera to the graphic
            OrbitGeoElementCameraController* cameraController = new OrbitGeoElementCameraController(graphic, 200, this);
            cameraController->setCameraPitchOffset(80);
            cameraController->setCameraHeadingOffset(-30);
            m_sceneView->setCameraController(cameraController);
        }
    });

    mms->load();

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_sceneView);
    setLayout(vBoxLayout);
}

