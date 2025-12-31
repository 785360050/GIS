#pragma once

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <Scene.h>
#include <SceneGraphicsView.h>
#include <IntegratedMeshLayer.h>
#include <Graphic.h>


class Layer_Mesh_Integrated : public QWidget
{
    Q_OBJECT
public slots:

private:
    Esri::ArcGISRuntime::Scene* m_scene = nullptr;
    Esri::ArcGISRuntime::SceneGraphicsView* m_sceneView = nullptr;
    Esri::ArcGISRuntime::IntegratedMeshLayer* m_integratedMeshLyr = nullptr;
    QString m_errorMessage;
public:
    explicit Layer_Mesh_Integrated(QWidget* parent = nullptr);



};

