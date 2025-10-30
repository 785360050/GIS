#pragma once

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <Scene.h>
#include <SceneGraphicsView.h>
#include <Graphic.h>


class Scene_Properties : public QWidget
{
    Q_OBJECT
public slots:

private:
    Esri::ArcGISRuntime::Scene* m_scene = nullptr;
    Esri::ArcGISRuntime::SceneGraphicsView* m_sceneView = nullptr;
    Esri::ArcGISRuntime::Graphic* m_graphic = nullptr;
    Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay=nullptr;
public:
    explicit Scene_Properties(QWidget* parent = nullptr);



};

