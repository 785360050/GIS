#pragma once

#include "Scene.h"
#include <Map.h>

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <QMainWindow>

class Shapefile : public QMainWindow
{
    Q_OBJECT
public:
    explicit Shapefile(QWidget* parent = nullptr);
    ~Shapefile() override;

public slots:

private:
    Esri::ArcGISRuntime::Scene* m_scene = nullptr;
    Esri::ArcGISRuntime::Map* m_map= nullptr;
    Esri::ArcGISRuntime::SceneGraphicsView* m_sceneView = nullptr;
};


