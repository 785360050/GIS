#pragma once

#include <Map.h>
#include <MapTypes.h>
#include <MapGraphicsView.h>

#include <QWidget>

class Geo_Package : public QWidget
{
    Q_OBJECT
private:
    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
public:
    explicit Geo_Package(QWidget* parent = nullptr);
};

