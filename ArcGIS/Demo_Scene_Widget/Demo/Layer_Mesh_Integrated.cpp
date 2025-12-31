// Other headers
#include "Layer_Mesh_Integrated.hpp"

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

Layer_Mesh_Integrated::Layer_Mesh_Integrated(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a scene using the ArcGISTerrain BasemapStyle
    m_scene = new Scene(BasemapStyle::ArcGISTerrain, this);

    // Create a scene view, and pass in the scene
    m_sceneView = new SceneGraphicsView(m_scene, this);

    m_integratedMeshLyr=new IntegratedMeshLayer(QUrl("https://tiles.arcgis.com/tiles/z2tnIkrLQ2BRzr6P/arcgis/rest/services/Girona_Spain/SceneServer"), this);
    m_scene->operationalLayers()->append(m_integratedMeshLyr);

    {
        /// ============================================================================================================
        /// 	如果没有高程数据源，IntegratedMeshLayer会浮空
        /// 如果添加高程数据源，不能更改ElevationExaggeration，不然会导致城市高度和地形高度不匹配。
        ///     地形高度比城市高的话会有渲染问题(类似关闭了深度测试的效果)
        /// ============================================================================================================

        // create a new elevation source from Terrain3D rest service
        ArcGISTiledElevationSource *elevationSource = new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

        auto surface = m_scene->baseSurface();
        surface->elevationSources()->append(elevationSource); // add the elevation source to the scene to display elevation
        // surface->setElevationExaggeration(2.5);
        m_scene->setBaseSurface(surface);
    }

    const Point initialPt(2.8259, 41.9906, 200.0, SpatialReference::wgs84());
    const Camera initialCamera(initialPt, 200.0, 190.0, 65.0, 0.0);
    const Viewpoint initialViewpoint(initialPt, initialPt.z(), initialCamera);
    m_scene->setInitialViewpoint(initialViewpoint);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_sceneView);
    setLayout(vBoxLayout);
}

