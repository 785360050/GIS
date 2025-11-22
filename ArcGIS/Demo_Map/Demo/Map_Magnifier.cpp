#include "Map_Magnifier.hpp"

#include <QVBoxLayout>

#include <Map.h>
#include <MapGraphicsView.h>
#include <MapTypes.h>
#include <Point.h>
#include <Viewpoint.h>
#include <SpatialReference.h>

using namespace Esri::ArcGISRuntime;

/// ============================================================================================================
/// 		鼠标长按地图显示放大镜，可能会卡一会才显示
/// ============================================================================================================

Map_Magnifier::Map_Magnifier(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a map using the ArcGISStreets BasemapStyle
    m_map = new Map(BasemapStyle::ArcGISImageryStandard, this);

    m_map->setInitialViewpoint(Viewpoint(Point(-110.8258, 32.1545089, SpatialReference::wgs84()), 2e4));


    // Create the Widget view
    m_mapView = new MapGraphicsView(this);
    m_mapView->setMagnifierEnabled(true);// enable magnifier
    m_mapView->setMap(m_map);// Set map to map view

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}


