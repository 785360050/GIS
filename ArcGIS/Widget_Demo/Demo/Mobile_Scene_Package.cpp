// Other headers
#include "Mobile_Scene_Package.hpp"

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
#include <MobileScenePackage.h>

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

Mobile_Scene_Package::Mobile_Scene_Package(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a scene using the ArcGISTerrain BasemapStyle
    m_scene = new Scene(BasemapStyle::ArcGISTerrain, this);

    // Create a scene view, and pass in the scene
    m_sceneView = new SceneGraphicsView(m_scene, this);

    // create the MSPK data path
    // data is downloaded automatically by the sample viewer app. Instructions to download
    // separately are specified in the readme.
    const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/mspk/philadelphia.mspk";

    // Create the Scene Package
    m_scenePackage = new MobileScenePackage(dataPath, this);
    // m_scenePackage = new MobileScenePackage(":/Resource/philadelphia.mspk", this);

    connect(m_scenePackage, &MobileScenePackage::doneLoading, this, &Mobile_Scene_Package::Package_Loaded_Finished);
    // connect to the Mobile Scene Package instance to know when errors occur
    connect(MobileScenePackage::instance(), &MobileScenePackage::errorOccurred,[](const Error& e)
    {
        if (e.isEmpty())
            return;

        qDebug() << QString("Error: %1 %2").arg(e.message(), e.additionalMessage());
    });

    m_scenePackage->load();

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_sceneView);
    setLayout(vBoxLayout);
}



void Mobile_Scene_Package::Package_Loaded_Finished(const Esri::ArcGISRuntime::Error &e)
{
    if (!e.isEmpty())
    {
        qDebug() << QString("Package load error: %1 %2").arg(e.message(), e.additionalMessage());
        return;
    }

    if (m_scenePackage->scenes().isEmpty())
        return;

    // The package contains a list of scenes that could be show in a UI for selection.
    // For simplicity, obtain the first scene in the list of scenes
    m_scene = m_scenePackage->scenes().at(0);

    // set the scene on the scene view to display
    if (m_scene && m_sceneView)
        m_sceneView->setArcGISScene(m_scene);
}

