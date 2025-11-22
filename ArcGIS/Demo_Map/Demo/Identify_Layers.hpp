#pragma once

#include <Map.h>
#include <MapTypes.h>
#include <MapGraphicsView.h>

#include <QWidget>

class Identify_Layers : public QWidget
{
    Q_OBJECT
private:
    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
public:
    explicit Identify_Layers(QWidget* parent = nullptr);


};

