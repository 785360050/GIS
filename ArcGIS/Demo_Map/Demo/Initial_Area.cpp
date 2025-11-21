#include "Initial_Area.hpp"

#include <QVBoxLayout>

#include <Map.h>
#include <MapGraphicsView.h>
#include <MapTypes.h>
#include <Envelope.h>
#include <Viewpoint.h>
#include <SpatialReference.h>

using namespace Esri::ArcGISRuntime;

Initial_Area::Initial_Area(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create envelope for the area of interest
    Envelope envelope(-12211308.778729, 4645116.003309, -12208257.879667, 4650542.535773, SpatialReference(102100));

    // Create a new map with the imagery with labels basemap
    m_map = new Map(BasemapStyle::ArcGISImagery, this);

    // Set the initial viewpoint to the envelope
    m_map->setInitialViewpoint(Viewpoint(envelope));

    // Create a map view, and pass in the map
    m_mapView = new MapGraphicsView(m_map, this);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}


