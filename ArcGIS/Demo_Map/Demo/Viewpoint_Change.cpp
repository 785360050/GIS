#include "Viewpoint_Change.hpp"

#include <QVBoxLayout>
#include <QFuture>

#include <Map.h>
#include <MapGraphicsView.h>
#include <MapTypes.h>
#include <Point.h>
#include <Envelope.h>
#include <Viewpoint.h>
#include <MapTypes.h>
#include "MapViewTypes.h"
#include <SpatialReference.h>



#include "../Utils/Signal_Proxy.hpp"

using namespace Esri::ArcGISRuntime;

Viewpoint_Change::Viewpoint_Change(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a map using an imagery basemap
    m_map = new Map(BasemapStyle::ArcGISImagery, this);

    // Create a map view, and pass in the map
    m_mapView = new MapGraphicsView(m_map, this);

    // Connect the combo box signal to slot for setting new viewpoint
    // connect(&Signal_Proxy::Instance(), &Signal_Proxy::Viewpoint_Change,this, &Viewpoint_Change::changeToNewViewpoint);


    static const Point ptEsriHeadquarters(-117.195681, 34.056218, SpatialReference(4236));
    static const Point ptHawaii(-157.564, 20.677, SpatialReference(4236));
    static const Envelope envBeijing(116.380, 39.920, 116.400, 39.940, SpatialReference(4236));
    static const Viewpoint vpSpring(Envelope(-12338668.348591767, 5546908.424239618, -12338247.594362013, 5547223.989911933, SpatialReference(102100)));

    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Viewpoint_Center,this, [&]()
    {
        m_mapView->setViewpointCenterAsync(ptEsriHeadquarters);
    });

    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Viewpoint_Center_scale,this, [&]()
            {
                m_mapView->setViewpointCenterAsync(ptHawaii, 4000000.0);
            });

    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Viewpoint_Geometry,this, [&]()
            {
                m_mapView->setViewpointGeometryAsync(envBeijing);
            });

    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Viewpoint_Geometry_Padding,this, [&]()
            {
                m_mapView->setViewpointGeometryAsync(envBeijing, 200 * screenRatio());
            });

    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Viewpoint_Rotation,this, [&]()
            {
                m_rotationValue = (m_rotationValue + 45) % 360;
                m_mapView->setViewpointRotationAsync(m_rotationValue);
            });

    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Viewpoint_Scale,this, [&](double value)
            {
                m_mapView->setViewpointScaleAsync(value);
            });

    connect(&Signal_Proxy::Instance(), &Signal_Proxy::Viewpoint_Scale_AnimationCurve,this, [&](int duration,int animate_type)
            {
                // animate_type=AnimationCurve::EaseInOutCubic;
                m_mapView->setViewpointAsync(vpSpring, duration, AnimationCurve::EaseInOutCubic);
            });



    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}



void Viewpoint_Change::changeToNewViewpoint(int index)
{
    // Create the objects used to change viewpoint in various cases

    Point ptEsriHeadquarters(-117.195681, 34.056218, SpatialReference(4236));
    Point ptHawaii(-157.564, 20.677, SpatialReference(4236));
    Envelope envBeijing(116.380, 39.920, 116.400, 39.940, SpatialReference(4236));
    Viewpoint vpSpring(Envelope(-12338668.348591767, 5546908.424239618, -12338247.594362013, 5547223.989911933, SpatialReference(102100)));

    switch (index)
    {
    // Call setViewpoint and pass in the appropriate viewpoint
    case 0: // "Center"
        m_mapView->setViewpointCenterAsync(ptEsriHeadquarters);
        break;
    case 1: // "Center and scale"
        m_mapView->setViewpointCenterAsync(ptHawaii, 4000000.0);
        break;
    case 2: // "Geometry"
        m_mapView->setViewpointGeometryAsync(envBeijing);
        break;
    case 3: // "Geometry and padding"
        m_mapView->setViewpointGeometryAsync(envBeijing, 200 * screenRatio());
        break;
    case 4: // "Rotation"
        m_rotationValue = (m_rotationValue + 45) % 360;
        m_mapView->setViewpointRotationAsync(m_rotationValue);
        break;
    case 5: // "Scale: 1:5,000,000"
        m_mapView->setViewpointScaleAsync(5000000.0);
        break;
    case 6: // "Scale: 1:10,000,000"
        m_mapView->setViewpointScaleAsync(10000000.0);
        break;
    case 7: // "Scale: 1:5,000,000"
        m_mapView->setViewpointAsync(vpSpring, 4.0, AnimationCurve::EaseInOutCubic);
        break;
    }
}

double Viewpoint_Change::screenRatio() const
{
    const double width = static_cast<double>(m_mapView->width());
    const double height = static_cast<double>(m_mapView->height());
    return height > width ? width / height : height / width;
}
