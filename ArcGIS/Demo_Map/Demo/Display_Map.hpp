#pragma once

namespace Esri::ArcGISRuntime {
class Map;
class MapGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <QMainWindow>

class Display_Map : public QWidget
{
    Q_OBJECT
private:
    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
public:
    explicit Display_Map(QWidget* parent = nullptr);
    ~Display_Map() override;
public slots:


};
