// Other headers
#include "Sync_Map_Scene.hpp"

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

Sync_Map_Scene::Sync_Map_Scene(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    m_scene=new Scene(BasemapStyle::ArcGISImageryStandard, this);
    m_sceneView = new SceneGraphicsView(m_scene, this);


    m_map=new Map(BasemapStyle::ArcGISImageryStandard, this);
    m_mapView=new MapGraphicsView(m_map,this);

    m_sceneView->setArcGISScene(m_scene);

    connect(m_sceneView, &SceneGraphicsView::viewpointChanged, this,
            [this]
            {
                if (m_mapView && m_sceneView->isNavigating())
                    m_mapView->setViewpointAsync(m_sceneView->currentViewpoint(ViewpointType::CenterAndScale), 0);
            });

    m_mapView->setMap(m_map);
    m_mapView->setRotationByPinchingEnabled(true);

    connect(m_mapView, &MapGraphicsView::viewpointChanged, this,
            [this]
            {
                if (m_sceneView && m_mapView->isNavigating())
                    m_sceneView->setViewpointAsync(m_mapView->currentViewpoint(ViewpointType::CenterAndScale), 0);
            });

    // Set up the UI
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_sceneView);
    layout->addWidget(m_mapView);
    setLayout(layout);
}

