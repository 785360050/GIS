#pragma once

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <Scene.h>
#include <GlobeCameraController.h>
#include <OrbitGeoElementCameraController.h>
#include <OrbitLocationCameraController.h>
#include <SceneGraphicsView.h>
#include <Graphic.h>


class Camera_Controller : public QWidget
{
    Q_OBJECT
public slots:

private:
    Esri::ArcGISRuntime::Scene* m_scene = nullptr;
    Esri::ArcGISRuntime::SceneGraphicsView* m_sceneView = nullptr;

    Esri::ArcGISRuntime::GlobeCameraController* m_orbitGlobe = nullptr;
    Esri::ArcGISRuntime::OrbitGeoElementCameraController* m_orbitGeoElement = nullptr;
    Esri::ArcGISRuntime::OrbitLocationCameraController* m_orbitLocation = nullptr;

    Esri::ArcGISRuntime::GraphicsOverlay* m_overlay;
public:
    explicit Camera_Controller(QWidget* parent = nullptr);



};

