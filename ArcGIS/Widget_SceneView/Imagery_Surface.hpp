#pragma once

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <Scene.h>
#include <SceneGraphicsView.h>


class Imagery_Surface : public QWidget
{
    Q_OBJECT
public slots:

private:
    Esri::ArcGISRuntime::Scene* m_scene = nullptr;
    Esri::ArcGISRuntime::SceneGraphicsView* m_sceneView = nullptr;
public:
    explicit Imagery_Surface(QWidget* parent = nullptr);
    ~Imagery_Surface() override;



};

