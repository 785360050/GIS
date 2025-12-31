#pragma once

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <Scene.h>
#include <SceneGraphicsView.h>
#include <ArcGISSceneLayer.h>
#include <Graphic.h>


class Layer_Selection : public QWidget
{
    Q_OBJECT
public slots:

private:
    Esri::ArcGISRuntime::Scene* m_scene = nullptr;
    Esri::ArcGISRuntime::SceneGraphicsView* m_sceneView = nullptr;
    Esri::ArcGISRuntime::ArcGISSceneLayer* m_sceneLayer = nullptr;
public:
    explicit Layer_Selection(QWidget* parent = nullptr);



};

