#include "Basemap_Change.hpp"

#include <QVBoxLayout>
#include <QComboBox>

#include <Map.h>
#include <MapGraphicsView.h>
#include <MapTypes.h>
#include <Basemap.h>

#include "../Utils/Signal_Proxy.hpp"

using namespace Esri::ArcGISRuntime;

Basemap_Change::Basemap_Change(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a map initially using the topographic basemap
    m_map = new Map(BasemapStyle::ArcGISTopographic, this);

    // Create a map view, and pass in the map
    m_mapView = new MapGraphicsView(m_map, this);

    // Connect the combo box signal to lambda for setting new basemaps
    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Basemap_Change, [this](int i)
    {
        switch (i)
        {
        // Call setBasemap and pass in the appropriate basemap
        case 0:
            m_map->setBasemap(new Basemap(BasemapStyle::ArcGISTopographic, this));
            break;
        case 1:
            m_map->setBasemap(new Basemap(BasemapStyle::ArcGISStreets, this));
            break;
        case 2:
            m_map->setBasemap(new Basemap(BasemapStyle::ArcGISImageryStandard, this));
            break;
        case 3:
            m_map->setBasemap(new Basemap(BasemapStyle::ArcGISOceans, this));
            break;
        }
    });

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

Basemap_Change::~Basemap_Change() = default;
