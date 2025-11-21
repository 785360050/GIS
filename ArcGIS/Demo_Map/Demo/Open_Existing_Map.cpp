#include "Open_Existing_Map.hpp"

#include <QVBoxLayout>
#include <QUrl>

#include <Map.h>
#include <MapGraphicsView.h>
#include <MapTypes.h>
#include <PortalItem.h>

#include "../Utils/Signal_Proxy.hpp"

using namespace Esri::ArcGISRuntime;

static void createPortalMaps(QMap<QString, QString> & m_portalIds)
{
    m_portalIds.insert("Population Pressure", "392451c381ad4109bf04f7bd442bc038");
    m_portalIds.insert("Terrestrial Ecosystems of the World", "5be0bc3ee36c4e058f7b3cebc21c74e6");
    m_portalIds.insert("Geology of United States", "92ad152b9da94dee89b9e387dfe21acd");
}

Open_Existing_Map::Open_Existing_Map(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a map view
    m_mapView = new MapGraphicsView(this);

    // Create the portal items
    createPortalMaps(m_portalIds);


    // Connect the input dialog accepted signal to lambda for opening the selected map
    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Existing_Map_Open, [this](const QString& map_name)
            {
                // create a portal item with the item id
                QString portalId = m_portalIds.value(map_name);
                if(portalId.isEmpty())
                    return;

                PortalItem* portalItem = new PortalItem(QUrl("https://arcgis.com/sharing/rest/content/items/" + portalId), this);

                // create a new map from the portal item
                Map* map = new Map(portalItem, this);

                // set the map to the map view
                m_mapView->setMap(map);

                map->load();
            }
            );

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}


