// Other headers
#include "Surface_Placement.hpp"

// C++ API headers
#include "ArcGISSceneLayer.h"
#include "ArcGISTiledElevationSource.h"
#include "Camera.h"
#include "ElevationSourceListModel.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LayerListModel.h"
#include "LayerSceneProperties.h"
#include "MapTypes.h"
#include "Point.h"
#include "Scene.h"
#include "SceneViewTypes.h"
#include "SimpleMarkerSymbol.h"
#include "SpatialReference.h"
#include "Surface.h"
#include "SymbolTypes.h"
#include "TextSymbol.h"
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

Surface_Placement::Surface_Placement(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a scene using the ArcGISTerrain BasemapStyle
    m_scene = new Scene(BasemapStyle::ArcGISImageryStandard, this);

    // Create a scene view, and pass in the scene
    m_sceneView = new SceneGraphicsView(m_scene, this);

    Surface* surface = new Surface(this);
    surface->elevationSources()->append(new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this));
    m_scene->setBaseSurface(surface);

    // Create scene layer from the Brest, France scene server.
    ArcGISSceneLayer* sceneLayer = new ArcGISSceneLayer(QUrl("https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/Buildings_Brest/SceneServer"), this);
    m_scene->operationalLayers()->append(sceneLayer);
    //! [Create Scene API snippet]

    // set an initial viewpoint
    const Point initialViewPoint(-4.45968, 48.3889, 37.9922);
    const Camera camera(initialViewPoint, 329.91, 96.6632, 0);
    const Viewpoint viewpoint(initialViewPoint, camera);
    m_scene->setInitialViewpoint(viewpoint);

    // set the scene to the scene view
    m_sceneView->setArcGISScene(m_scene);

    Add_Graphics();

    // set the sceneView as the central widget
    // setCentralWidget(m_sceneView);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_sceneView);
    setLayout(vBoxLayout);

    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Surface_Placement_Change_Draped_Visibility,this,&Surface_Placement::Change_Draped_Visibility);
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Surface_Placement_Change_Z,this,&Surface_Placement::Change_ZValue);
}


void Surface_Placement::Add_Graphics()
{
    // add graphics overlays and set surface placement
    {
        // Graphics overlay with draped billboarded surface placement
        m_drapedBillboardedOverlay = new GraphicsOverlay(this);
        m_drapedBillboardedOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::DrapedBillboarded));
        m_sceneView->graphicsOverlays()->append(m_drapedBillboardedOverlay);

        // Graphics overlay with draped flat surface placement
        m_drapedFlatOverlay = new GraphicsOverlay(this);
        m_drapedFlatOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::DrapedFlat));
        m_drapedFlatOverlay->setVisible(false);
        m_sceneView->graphicsOverlays()->append(m_drapedFlatOverlay);

        // Graphics overlay with relative surface placement
        m_relativeOverlay = new GraphicsOverlay(this);
        m_relativeOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Relative));
        m_sceneView->graphicsOverlays()->append(m_relativeOverlay);

        // Graphics overlay with relative to scene surface placement
        m_relativeToSceneOverlay = new GraphicsOverlay(this);
        m_relativeToSceneOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::RelativeToScene));
        m_sceneView->graphicsOverlays()->append(m_relativeToSceneOverlay);

        // Graphics overlay with absolute surface placement
        m_absoluteOverlay = new GraphicsOverlay(this);
        m_absoluteOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Absolute));
        m_sceneView->graphicsOverlays()->append(m_absoluteOverlay);
    }


    // create point for the scene related graphic with a z value of 70
    const Point sceneRelatedPoint(-4.4610562, 48.3902727, 70, SpatialReference::wgs84());

    // create point for the surface related graphics with z value of 70
    const Point surfaceRelatedPoint(-4.4609257, 48.3903965 , 70, SpatialReference::wgs84());

    // create simple marker symbol
    static SimpleMarkerSymbol* simpleMarkerSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Triangle, QColor("red"), 20, this);

    // create text symbols
    TextSymbol* drapedBillboardedText = new TextSymbol("DRAPED BILLBOARDED", QColor("blue"), 20, HorizontalAlignment::Left, VerticalAlignment::Middle, this);
    drapedBillboardedText->setOffsetX(20);
    // Graphics will be draped on the surface of the scene and will always face the camera.
    m_drapedBillboardedOverlay->graphics()->append(new Graphic(surfaceRelatedPoint, simpleMarkerSymbol));
    m_drapedBillboardedOverlay->graphics()->append(new Graphic(surfaceRelatedPoint, drapedBillboardedText));

    TextSymbol* drapedFlatText = new TextSymbol("DRAPED FLAT", QColor("blue"), 20, HorizontalAlignment::Left, VerticalAlignment::Middle, this);
    drapedFlatText->setOffsetX(20);
    // Graphics will be draped on the surface of the scene
    m_drapedFlatOverlay->graphics()->append(new Graphic(surfaceRelatedPoint, simpleMarkerSymbol));
    m_drapedFlatOverlay->graphics()->append(new Graphic(surfaceRelatedPoint, drapedFlatText));

    TextSymbol* relativeText = new TextSymbol("RELATIVE", QColor("blue"), 20, HorizontalAlignment::Left, VerticalAlignment::Middle, this);
    relativeText->setOffsetX(20);
    // Graphics will be placed at z value relative to the surface
    m_relativeOverlay->graphics()->append(new Graphic(surfaceRelatedPoint, simpleMarkerSymbol));
    m_relativeOverlay->graphics()->append(new Graphic(surfaceRelatedPoint, relativeText));

    TextSymbol* relativeToSceneText = new TextSymbol("RELATIVE TO SCENE", QColor("blue"), 20, HorizontalAlignment::Right, VerticalAlignment::Middle, this);
    relativeToSceneText->setOffsetX(-20);
    // Graphics will be placed at z value relative to the scene
    m_relativeToSceneOverlay->graphics()->append(new Graphic(sceneRelatedPoint, simpleMarkerSymbol));
    m_relativeToSceneOverlay->graphics()->append(new Graphic(sceneRelatedPoint, relativeToSceneText));

    TextSymbol* absoluteText = new TextSymbol("ABSOLUTE", QColor("blue"), 20, HorizontalAlignment::Left, VerticalAlignment::Middle, this);
    absoluteText->setOffsetX(20);
    // Graphics will be placed at absolute z value
    m_absoluteOverlay->graphics()->append(new Graphic(surfaceRelatedPoint, simpleMarkerSymbol));
    m_absoluteOverlay->graphics()->append(new Graphic(surfaceRelatedPoint, absoluteText));

}

void Surface_Placement::Change_Draped_Visibility()
{
    // m_drapedFlatOverlay->setVisible(visible);
    // m_drapedBillboardedOverlay->setVisible(visible);
    m_drapedFlatOverlay->setVisible(!m_drapedFlatOverlay->isVisible());
    m_drapedBillboardedOverlay->setVisible(!m_drapedBillboardedOverlay->isVisible());
}

void Surface_Placement::Change_ZValue(double zValue)
{
    for (GraphicsOverlay* overlay : *m_sceneView->graphicsOverlays())
    {
        for (Graphic* graphic : *overlay->graphics())
        {
            // create new graphic with the same existing information but a new Z-value
            const Point graphicPoint{graphic->geometry()};
            const Point point{graphicPoint.x(), graphicPoint.y(), zValue, graphicPoint.spatialReference()};
            graphic->setGeometry(point);
        }
    }
}
