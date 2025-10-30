#pragma once

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <Scene.h>
#include <SceneGraphicsView.h>
#include <Graphic.h>


class Scene_Symbol : public QWidget
{
    Q_OBJECT
public slots:

private:
    Esri::ArcGISRuntime::Scene* m_scene = nullptr;
    Esri::ArcGISRuntime::SceneGraphicsView* m_sceneView = nullptr;
public:
    explicit Scene_Symbol(QWidget* parent = nullptr);
    ~Scene_Symbol(){};



};

