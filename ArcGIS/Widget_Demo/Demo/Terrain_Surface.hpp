#pragma once

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <Scene.h>
#include <SceneGraphicsView.h>
#include <Graphic.h>


class Terrain_Surface : public QWidget
{
    Q_OBJECT
public slots:

private:
    Esri::ArcGISRuntime::Scene* m_scene = nullptr;
    Esri::ArcGISRuntime::SceneGraphicsView* m_sceneView = nullptr;
public:
    explicit Terrain_Surface(QWidget* parent = nullptr);
    ~Terrain_Surface() override;



};

