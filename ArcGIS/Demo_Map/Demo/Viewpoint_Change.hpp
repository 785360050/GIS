#pragma once

namespace Esri::ArcGISRuntime {
class Map;
class MapGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <QMainWindow>

class Viewpoint_Change : public QWidget
{
    Q_OBJECT
private slots:
    void changeToNewViewpoint(int);


private:
    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
    // QComboBox* m_viewpointCombo = nullptr;
    int m_rotationValue = 0;
public:
    explicit Viewpoint_Change(QWidget* parent = nullptr);

private:
    double screenRatio() const;

};

