#include "Geo_Package.hpp"

#include <QVBoxLayout>
#include <QStandardPaths>
#include <QFuture>

#include <Error.h>
#include <FeatureLayer.h>
#include <FeatureTable.h>
#include <GeoPackage.h>
#include <GeoPackageFeatureTable.h>
#include <GeoPackageRaster.h>
#include <Layer.h>
#include <Viewpoint.h>
#include <LayerListModel.h>
#include <Map.h>
#include <MapTypes.h>
#include <RasterLayer.h>

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
} // namespace


static QString m_dataPath=defaultDataPath() + "/ArcGIS/Runtime/Data";

Geo_Package::Geo_Package(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a map using the topographic basemap
    m_map = new Map(BasemapStyle::ArcGISTopographic, this);

    // Create the Widget view
    m_mapView = new MapGraphicsView(this);

    // Set map to map view
    m_mapView->setMap(m_map);

    // Create the initial view area
    const Viewpoint initialViewpoint(39.7294, -104.8319, 500000);
    m_mapView->setViewpointAsync(initialViewpoint);

    // Read the GeoPackage and create lists for the names and layer data
    // Load the GeoPackage at the beginning
    GeoPackage* auroraGpkg = new GeoPackage(m_dataPath + "/gpkg/AuroraCO.gpkg", this);

    // Make sure there are no errors in loading the GeoPackage before interacting with it
    connect(auroraGpkg, &GeoPackage::doneLoading, this, [auroraGpkg, this](const Error& error)
            {
                if (error.isEmpty())
                {
                    // For each raster, create a raster layer and add the layer to the map
                    const auto rasters = auroraGpkg->geoPackageRasters();
                    for (GeoPackageRaster* rasterItm : rasters)
                    {
                        RasterLayer* rasterLyr = new RasterLayer(rasterItm, this);
                        rasterLyr->setOpacity(0.55f);
                        m_map->operationalLayers()->append(rasterLyr);
                    }

                    // For each feature table, create a feature layer and add the layer to the map
                    const auto tables = auroraGpkg->geoPackageFeatureTables();
                    for (GeoPackageFeatureTable* featureTbl : tables)
                    {
                        FeatureLayer* layer = new FeatureLayer(featureTbl, this);
                        m_map->operationalLayers()->append(layer);
                    }
                }
            });

    auroraGpkg->load();

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}



