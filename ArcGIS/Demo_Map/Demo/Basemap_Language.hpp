#pragma once

#include <Basemap.h>
#include <Map.h>
#include <MapGraphicsView.h>
#include <SpatialReference.h>
#include <BasemapStyleParameters.h>

#include <QMainWindow>

class Basemap_Language : public QWidget
{
    Q_OBJECT
private:
    Esri::ArcGISRuntime::Map* m_map = new Esri::ArcGISRuntime::Map(Esri::ArcGISRuntime::SpatialReference::webMercator(), this);
    Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
    Esri::ArcGISRuntime::BasemapStyleParameters* m_basemapStyleParameters = new Esri::ArcGISRuntime::BasemapStyleParameters(this);
    Esri::ArcGISRuntime::Basemap* m_basemap = nullptr;
public:
    explicit Basemap_Language(QWidget* parent = nullptr);
public:
    void Set_Basemap_Language(const QString& language);

};

