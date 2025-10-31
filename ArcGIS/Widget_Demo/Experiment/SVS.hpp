#pragma once


#include <Scene.h>
#include <SceneGraphicsView.h>
#include <Map.h>
#include <MapGraphicsView.h>
#include <Graphic.h>
#include "MapTypes.h"

class SVS : public QWidget
{
    Q_OBJECT
public slots:

private:
    Esri::ArcGISRuntime::Scene* scene {new Esri::ArcGISRuntime::Scene(Esri::ArcGISRuntime::BasemapStyle::ArcGISTerrain, this)};
    Esri::ArcGISRuntime::SceneGraphicsView* scene_view {new Esri::ArcGISRuntime::SceneGraphicsView(scene, this)};

    Esri::ArcGISRuntime::Map* map {new Esri::ArcGISRuntime::Map(Esri::ArcGISRuntime::BasemapStyle::ArcGISImageryStandard, this)};
    Esri::ArcGISRuntime::MapGraphicsView* map_view {new Esri::ArcGISRuntime::MapGraphicsView(map,this)};
public:
    explicit SVS(QWidget* parent = nullptr);



};

