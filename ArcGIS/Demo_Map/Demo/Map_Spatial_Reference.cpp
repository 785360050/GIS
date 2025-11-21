#include "Map_Spatial_Reference.hpp"

#include <QVBoxLayout>
#include <QUrl>

#include <Map.h>
#include <MapGraphicsView.h>
#include <MapTypes.h>
#include <SpatialReference.h>

using namespace Esri::ArcGISRuntime;

static MapGraphicsView* Create_Map_View(SpatialReference spatial_reference,QWidget* parent)
{
    // Create a new map with the spatial reference
    auto m_map = new Map(spatial_reference, parent);

    // create the URL pointing to the map image layer
    QUrl imageLayerUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/SampleWorldCities/MapServer");

    // construct the ArcGISMapImageLayer using the URL
    auto m_imageLayer = new ArcGISMapImageLayer(imageLayerUrl, parent);

    // create a Basemap and pass in the ArcGISMapImageLayer
    auto m_basemap = new Basemap(m_imageLayer, parent);
    // auto m_basemap = new Basemap(BasemapStyle::ArcGISImagery, parent);

    //Set the ArcGISMapImageLayer as basemap
    m_map->setBasemap(m_basemap);

    // Create a map view, and pass in the map
    auto m_mapView = new MapGraphicsView(m_map, parent);
    return m_mapView;
}

Map_Spatial_Reference::Map_Spatial_Reference(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{


    // Set up the UI
    QHBoxLayout *layout = new QHBoxLayout(this);
    // 1. Web Mercator（常用在线地图）
    layout->addWidget(Create_Map_View(SpatialReference::webMercator(), this));

    // 2. WGS84 地理坐标系（EPSG:4326）
    layout->addWidget(Create_Map_View(SpatialReference::wgs84(), this));

    // 3. CGCS2000（国内常用，EPSG:4490）
    layout->addWidget(Create_Map_View(SpatialReference(4490), this));

    // 4. UTM 投影示例（这里示例 32650：WGS84 / UTM zone 50N）
    layout->addWidget(Create_Map_View(SpatialReference(32650), this));

    // 5. 高斯-克吕格中国常用投影示例（EPSG:4547 CGCS2000 / Gauss-Kruger zone 13）
    layout->addWidget(Create_Map_View(SpatialReference(4547), this));

    // 6. 一个投影坐标系（EPSG:54024 World Mercator）
    layout->addWidget(Create_Map_View(SpatialReference(54024), this));

    setLayout(layout);
}


