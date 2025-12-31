#pragma once

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <Scene.h>
#include <SceneGraphicsView.h>
#include <Graphic.h>

#include <QFuture>


class Point_Elevation : public QWidget
{
    Q_OBJECT
public slots:

private:
    Esri::ArcGISRuntime::Scene* m_scene = nullptr;
    Esri::ArcGISRuntime::SceneGraphicsView* m_sceneView = nullptr;

    Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
    Esri::ArcGISRuntime::Graphic* m_elevationMarker = nullptr;
public:
    explicit Point_Elevation(QWidget* parent = nullptr);

protected:
    void displayElevationOnClick(QMouseEvent& mouseEvent);
    double m_elevation = 0.0;
    QFuture<double> m_elevationQueryFuture;

};

