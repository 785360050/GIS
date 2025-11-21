
#include <QVBoxLayout>

// Other headers
#include "Display_Map.hpp"

// C++ API headers
#include <Map.h>
#include <MapGraphicsView.h>
#include <MapTypes.h>

#include "../../Utils/Signal_Proxy.hpp"

using namespace Esri::ArcGISRuntime;

Display_Map::Display_Map(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a map using the ArcGISStreets BasemapStyle
    m_map = new Map(BasemapStyle::ArcGISStreets, this);

    // Create the Widget view
    m_mapView = new MapGraphicsView(this);

    // Set map to map view
    m_mapView->setMap(m_map);


    // Map Load Status changed lambda
    connect(m_map, &Esri::ArcGISRuntime::Map::loadStatusChanged, &Signal_Proxy::Instance(),&Signal_Proxy::Dispaly_Map_Status_Update);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

Display_Map::~Display_Map() = default;
