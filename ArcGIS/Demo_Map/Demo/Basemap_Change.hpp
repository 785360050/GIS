#pragma once

namespace Esri::ArcGISRuntime {
class Map;
class MapGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <QMainWindow>

class Basemap_Change : public QWidget
{
    Q_OBJECT
private:
    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
public:
    explicit Basemap_Change(QWidget* parent = nullptr);
    ~Basemap_Change() override;

public slots:


};

