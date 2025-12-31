#pragma once

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <Scene.h>
#include <SceneGraphicsView.h>
#include <Graphic.h>
#include <MobileScenePackage.h>


class Mobile_Scene_Package : public QWidget
{
    Q_OBJECT
public slots:

private:
    Esri::ArcGISRuntime::Scene* m_scene = nullptr;
    Esri::ArcGISRuntime::SceneGraphicsView* m_sceneView = nullptr;
    Esri::ArcGISRuntime::MobileScenePackage* m_scenePackage = nullptr;
public:
    explicit Mobile_Scene_Package(QWidget* parent = nullptr);

protected:
    void Package_Loaded_Finished(const Esri::ArcGISRuntime::Error& e);

};

