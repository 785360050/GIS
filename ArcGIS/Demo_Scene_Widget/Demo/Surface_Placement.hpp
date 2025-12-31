#pragma once

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <Scene.h>
#include <SceneGraphicsView.h>
#include <Graphic.h>


class Surface_Placement : public QWidget
{
    Q_OBJECT
public slots:

private:
    Esri::ArcGISRuntime::Scene* m_scene = nullptr;
    Esri::ArcGISRuntime::SceneGraphicsView* m_sceneView = nullptr;

    Esri::ArcGISRuntime::GraphicsOverlay* m_drapedFlatOverlay = nullptr;
    Esri::ArcGISRuntime::GraphicsOverlay* m_drapedBillboardedOverlay = nullptr;
    Esri::ArcGISRuntime::GraphicsOverlay* m_relativeOverlay = nullptr;
    Esri::ArcGISRuntime::GraphicsOverlay* m_relativeToSceneOverlay = nullptr;
    Esri::ArcGISRuntime::GraphicsOverlay* m_absoluteOverlay = nullptr;
public:
    explicit Surface_Placement(QWidget* parent = nullptr);

protected:
    void Add_Graphics();
public:
    void Change_Draped_Visibility();

    void Change_ZValue(double zValue);

};



