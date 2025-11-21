#include "Viewpoint_Max_Extent.hpp"

#include <QVBoxLayout>

#include <Map.h>
#include <MapGraphicsView.h>
#include <MapTypes.h>
#include <SimpleLineSymbol.h>
#include <SpatialReference.h>

#include <GraphicListModel.h>
#include <GraphicsOverlayListModel.h>
#include <GraphicsOverlay.h>
#include <SymbolTypes.h>

using namespace Esri::ArcGISRuntime;

Viewpoint_Max_Extent::Viewpoint_Max_Extent(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a map using the ArcGISStreets BasemapStyle
    m_map = new Map(BasemapStyle::ArcGISStreets, this);

    // Create the Widget view
    m_mapView = new MapGraphicsView(this);

    // Set map to map view
    m_mapView->setMap(m_map);

    {
        m_coloradoOverlay = new GraphicsOverlay(this);
        // Create an envelope to be used as the Colorado geometry
        m_coloradoEnvelope = Envelope(-12138232, 4441198, -11362327, 5012861, SpatialReference(3857));

        m_mapView->graphicsOverlays()->append(m_coloradoOverlay);
        m_map->setMaxExtent(m_coloradoEnvelope);
        Create_Extent_Border();

    }

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

void Viewpoint_Max_Extent::Create_Extent_Border()
{
    m_coloradoOutline = new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, QColor("red"), 3.0f, this);
    m_coloradoGraphic = new Esri::ArcGISRuntime::Graphic(m_coloradoEnvelope, m_coloradoOutline, this);
    m_coloradoOverlay->graphics()->append(m_coloradoGraphic);
}


