#pragma once

#include <Map.h>
#include <MapTypes.h>
#include <Graphic.h>
#include <Point.h>
#include <GraphicsOverlay.h>
#include <SimulatedLocationDataSource.h>
#include <PolylineBuilder.h>
#include <MapGraphicsView.h>

#include <QWidget>

class Location_History : public QWidget
{
    Q_OBJECT
private:
    Esri::ArcGISRuntime::Map* m_map = new Esri::ArcGISRuntime::Map(Esri::ArcGISRuntime::BasemapStyle::ArcGISDarkGray, this);
    Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
    Esri::ArcGISRuntime::GraphicsOverlay* m_locationHistoryOverlay = new Esri::ArcGISRuntime::GraphicsOverlay(this);
    Esri::ArcGISRuntime::GraphicsOverlay* m_locationHistoryLineOverlay = new Esri::ArcGISRuntime::GraphicsOverlay(this);
    Esri::ArcGISRuntime::Graphic* m_locationHistoryLineGraphic = nullptr;
    Esri::ArcGISRuntime::PolylineBuilder* m_polylineBuilder = nullptr;
    Esri::ArcGISRuntime::SimulatedLocationDataSource* m_simulatedLocationDataSource = nullptr;
    Esri::ArcGISRuntime::Point m_lastPosition;
    bool m_trackingEnabled = false;
public:
    explicit Location_History(QWidget* parent = nullptr);
public:

    void Config_Location_Changes();
};

