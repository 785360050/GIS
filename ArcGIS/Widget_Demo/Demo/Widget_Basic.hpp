#pragma once

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <Scene.h>
#include <SceneGraphicsView.h>
#include <Graphic.h>


class Widget_Basic : public QWidget
{
    Q_OBJECT
public:
    explicit Widget_Basic(QWidget* parent = nullptr);
private:
    Esri::ArcGISRuntime::SceneGraphicsView* Initialize_Widget(Esri::ArcGISRuntime::BasemapStyle basemap_style);


};

