#include "Basemap_Language.hpp"

#include <QVBoxLayout>
#include <QFuture>

#include <Map.h>
#include <MapGraphicsView.h>
#include <MapTypes.h>
#include <Point.h>
#include <Viewpoint.h>

#include "../Utils/Signal_Proxy.hpp"

using namespace Esri::ArcGISRuntime;

Basemap_Language::Basemap_Language(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a map using the ArcGISStreets BasemapStyle
    m_map = new Map(BasemapStyle::ArcGISStreets, this);

    // Create the Widget view
    m_mapView = new MapGraphicsView(this);

    // Set map to map view
    m_mapView->setMap(m_map);


    const Point point(3144804, 4904598);

    // Create a Viewpoint object with the created Point and a scale of 10000000
    const Viewpoint viewpoint(point, 10000000);

    // Set the viewpoint for the mapView
    m_mapView->setViewpointAsync(viewpoint);

    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Basemap_Language_Global,this,[&](bool is_global){m_basemapStyleParameters->setLanguageStrategy(is_global ? BasemapStyleLanguageStrategy::Global : BasemapStyleLanguageStrategy::Local);});
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Basemap_Language_Select,this,&Basemap_Language::Set_Basemap_Language);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}


// Set new basemap language based on the parameters selected
void Basemap_Language::Set_Basemap_Language(const QString& language)
{
    // A SpecificLanguage setting overrides the LanguageStrategy settings
    if (language == "none")
        m_basemapStyleParameters->setSpecificLanguage("");
    else if (language == "Bulgarian")
        m_basemapStyleParameters->setSpecificLanguage("bg");
    else if (language == "Greek")
        m_basemapStyleParameters->setSpecificLanguage("el");
    else if (language == "Turkish")
        m_basemapStyleParameters->setSpecificLanguage("tr");

    if (m_basemap)
        m_basemap->deleteLater();

    m_basemap = new Basemap(BasemapStyle::ArcGISLightGray, m_basemapStyleParameters, this);
    m_map->setBasemap(m_basemap);
}
