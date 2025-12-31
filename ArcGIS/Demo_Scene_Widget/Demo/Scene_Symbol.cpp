// Other headers
#include "Scene_Symbol.hpp"

// C++ API headers
#include "ArcGISTiledElevationSource.h"
#include "ElevationSourceListModel.h"
#include "RasterElevationSource.h"
#include "MapTypes.h"
#include "GraphicsOverlay.h"
#include "SpatialReference.h"
#include "Scene.h"
#include "Viewpoint.h"
#include "Point.h"
#include "SceneGraphicsView.h"
#include "Surface.h"

#include "ArcGISTiledElevationSource.h"
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
#include "Scene.h"
#include "SceneViewTypes.h"
#include "SimpleMarkerSceneSymbol.h"
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




Scene_Symbol::Scene_Symbol(QWidget *parent)
    : QWidget(parent)
{
    // Create a scene using the ArcGISTerrain BasemapStyle
    m_scene = new Scene(BasemapStyle::ArcGISImageryStandard, this);

    // Create a scene view, and pass in the scene
    m_sceneView = new SceneGraphicsView(m_scene, this);

    // create a new elevation source
    ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(QString{defaultDataPath() + "/ArcGIS/Runtime/Data/tpkx/MontereyElevation.tpkx"}, this);
    // add the elevation source to the scene to display elevation
    m_scene->baseSurface()->elevationSources()->append(elevationSource);

    // create the MontereyElevation data path
    // data is downloaded automatically by the sample viewer app. Instructions to download
    // separately are specified in the readme.
    const QString montereyRasterElevationPath = QString{defaultDataPath() + "/ArcGIS/Runtime/Data/raster/MontereyElevation.dt2"};

    //Before attempting to add any layers, check that the file for the elevation source exists at all.
    const bool srcElevationFileExists = QFileInfo::exists(montereyRasterElevationPath);

    if(srcElevationFileExists)
    {
        //Create the elevation source from the local raster(s). RasterElevationSource can take multiple files as inputs, but in this case only takes one.
        RasterElevationSource* elevationSrc = new RasterElevationSource{QStringList{montereyRasterElevationPath}, this};

        //When the elevation source is finished loading, call the elevationSrcFinishedLoading callback, so we can tell if it loaded succesfully.
        connect(elevationSrc, &RasterElevationSource::doneLoading, this, [&](const Error& loadError)
                {
                    if(loadError.isEmpty())
                    {
                        //Succesful load
                        qInfo() << "Loaded raster elevation source succesfully";
                    }
                    else {
                        //Log failure to load
                        qWarning() << "Error loading elevation source : " << loadError.message();
                    }
                });

        // add the elevation source to the scene to display elevation
        m_scene->baseSurface()->elevationSources()->append(elevationSrc);
    }
    else
    {
        qWarning() << "Could not find file at : " << montereyRasterElevationPath << ". Elevation source not set.";
    }

    // create a camera
    Camera camera(34, 45, 6000, 0, 0, 0);
    // set the viewpoint
    m_sceneView->setViewpointCameraAndWait(camera);

    // create a graphics overlay
    GraphicsOverlay* graphicsOverlay = new GraphicsOverlay(this);
    graphicsOverlay->setSceneProperties(LayerSceneProperties(Esri::ArcGISRuntime::SurfacePlacement::Absolute));
    // add the graphics overlay to the scene view
    m_sceneView->graphicsOverlays()->append(graphicsOverlay);

    // create a map of the symbol styles and color
    QMap<SimpleMarkerSceneSymbolStyle, QColor> symbolStyles;
    symbolStyles.insert(SimpleMarkerSceneSymbolStyle::Cone, QColor("red"));
    symbolStyles.insert(SimpleMarkerSceneSymbolStyle::Cube, QColor("white"));
    symbolStyles.insert(SimpleMarkerSceneSymbolStyle::Cylinder, QColor("purple"));
    symbolStyles.insert(SimpleMarkerSceneSymbolStyle::Diamond, QColor("turquoise"));
    symbolStyles.insert(SimpleMarkerSceneSymbolStyle::Sphere, QColor("blue"));
    symbolStyles.insert(SimpleMarkerSceneSymbolStyle::Tetrahedron, QColor("yellow"));
    QMapIterator<SimpleMarkerSceneSymbolStyle, QColor> i(symbolStyles);

    constexpr double x = 44.975;
    constexpr double y = 34.0;
    constexpr double z = 500.0;
    int counter = 0;

    // iterate over the map
    while (i.hasNext())
    {
        i.next();
        // create a scene symbol based on the current type
        SimpleMarkerSceneSymbol* smss = new SimpleMarkerSceneSymbol(i.key(), i.value(), 200, 200, 200, SceneSymbolAnchorPosition::Center, this);
        // create a graphic using the symbol above and a point location
        Graphic* graphic = new Graphic(Point(x + 0.01 * counter++, y, z, m_sceneView->spatialReference()), smss, this);
        // add the graphic to the graphics overlay
        graphicsOverlay->graphics()->append(graphic);
    }

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_sceneView);
    setLayout(vBoxLayout);
}
