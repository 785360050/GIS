#include "Mobile_Map_Package.hpp"

#include <QVBoxLayout>
#include <QStandardPaths>

#include <Map.h>
#include <MapGraphicsView.h>
#include <MapTypes.h>
#include <Error.h>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
QString defaultDataPath()
{
    QString dataPath;

#ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

    return dataPath;
}

// sample MMPK location
const QString sampleFileYellowstone {"/ArcGIS/Runtime/Data/mmpk/Yellowstone.mmpk"};

} // namespace


Mobile_Map_Package::Mobile_Map_Package(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    m_mapView = new MapGraphicsView(this);




    // create the MMPK data path
    // data is downloaded automatically by the sample viewer app. Instructions to download
    // separately are specified in the readme.
    const QString dataPath = defaultDataPath() + sampleFileYellowstone;

    //! [open mobile map package cpp snippet]
    // Load the MMPK
    // instatiate a mobile map package
    m_mobileMapPackage = new MobileMapPackage(dataPath, this);

    // wait for the mobile map package to load
    connect(m_mobileMapPackage, &MobileMapPackage::doneLoading, this, &Mobile_Map_Package::Load_Map);
    // connect to the Mobile Map Package instance to know when errors occur
    connect(MobileMapPackage::instance(), &MobileMapPackage::errorOccurred,[](const Error& e)
            {
                if (e.isEmpty())
                    return;
                qDebug() << QString("Error: %1 %2").arg(e.message(), e.additionalMessage());
            });

    m_mobileMapPackage->load();
    //! [open mobile map package cpp snippet]

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

void Mobile_Map_Package::Load_Map(const Esri::ArcGISRuntime::Error &error)
{
    if (!error.isEmpty())
    {
        qDebug() << QString("Package load error: %1 %2").arg(error.message(), error.additionalMessage());
        return;
    }

    if (!m_mobileMapPackage || !m_mapView || m_mobileMapPackage->maps().isEmpty())
    {
        qDebug() << QString("nullptr error");
        return;
    }

    // The package contains a list of maps that could be shown in the UI for selection.
    // For simplicity, obtain the first map in the list of maps.
    // set the map on the map view to display
    m_mapView->setMap(m_mobileMapPackage->maps().at(0));

}




