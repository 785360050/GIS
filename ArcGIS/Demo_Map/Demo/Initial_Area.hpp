#pragma once

namespace Esri::ArcGISRuntime {
class Map;
class MapGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <QMainWindow>

class Initial_Area : public QWidget
{
    Q_OBJECT
private:
    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
public:
    explicit Initial_Area(QWidget* parent = nullptr);

};

