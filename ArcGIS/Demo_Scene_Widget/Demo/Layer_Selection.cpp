// Other headers
#include "Layer_Selection.hpp"

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
#include "GeoElement.h"
#include "IdentifyLayerResult.h"

#include "Feature.h"

#include <Camera.h>
#include <ArcGISSceneLayer.h>
#include <LayerListModel.h>
#include <Error.h>

#include <QUrl>
#include <QStandardPaths>
#include <QFileInfo>
#include <QFuture>
#include <QMouseEvent>
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

Layer_Selection::Layer_Selection(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a scene using the ArcGISTerrain BasemapStyle
    m_scene = new Scene(BasemapStyle::ArcGISTopographic, this);

    // Create a scene view, and pass in the scene
    m_sceneView = new SceneGraphicsView(m_scene, this);

    // add a surface
    Surface* surface = new Surface(this);
    surface->elevationSources()->append(new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"),this));
    m_scene->setBaseSurface(surface);

    // add a scene layer
    m_sceneLayer = new ArcGISSceneLayer(QUrl("https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/Buildings_Brest/SceneServer/layers/0"), this);
    m_scene->operationalLayers()->append(m_sceneLayer);

    // Set an initial viewpoint
    Point pt(-4.49779155626782, 48.38282454039932, 62.013264927081764, SpatialReference(4326));
    Camera camera(pt, 41.64729875588979, 71.2017391571523, 2.194677223e-314);
    Viewpoint initViewpoint(pt, camera);
    m_scene->setInitialViewpoint(initViewpoint);

    // when the scene is clicked, identify the clicked feature and select it
    connect(m_sceneView, &SceneGraphicsView::mouseClicked, this, [this](const QMouseEvent& mouseEvent)
            {
                // clear any previous selection
                m_sceneLayer->clearSelection();

                // identify from the click
                m_sceneView->identifyLayerAsync(m_sceneLayer, mouseEvent.position(), 10, false).then(this,
                                                                                                     [this](IdentifyLayerResult* result)
                                                                                                     {
                                                                                                         // get the results
                                                                                                         QList<GeoElement*> geoElements = result->geoElements();

                                                                                                         // make sure we have at least 1 GeoElement
                                                                                                         if (geoElements.isEmpty())
                                                                                                             return;

                                                                                                         // get the first GeoElement
                                                                                                         GeoElement* geoElement = geoElements.first();

                                                                                                         // cast the GeoElement to a Feature
                                                                                                         Feature* feature = static_cast<Feature*>(geoElement);

                                                                                                         // select the Feature in the SceneLayer
                                                                                                         if (feature)
                                                                                                         {
                                                                                                             feature->setParent(this);
                                                                                                             m_sceneLayer->selectFeature(feature);
                                                                                                         }
                                                                                                     });
            });

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_sceneView);
    setLayout(vBoxLayout);
}

