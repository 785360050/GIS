#include "Initial_Location.hpp"

#include <QVBoxLayout>

#include <Map.h>
#include <MapGraphicsView.h>
#include <MapTypes.h>
#include <Viewpoint.h>

using namespace Esri::ArcGISRuntime;

Initial_Location::Initial_Location(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a new map with the imagery basemap style enum and set its initial lat, long, and scale
    constexpr double lat = -33.867886;
    constexpr double lon = -63.985;
    constexpr double scale = 9027.977411;

    m_map = new Map(BasemapStyle::ArcGISImagery, this);
    m_map->setInitialViewpoint(Viewpoint(lat, lon, scale));

    // Create a map view, and pass in the map
    m_mapView = new MapGraphicsView(m_map, this);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}


