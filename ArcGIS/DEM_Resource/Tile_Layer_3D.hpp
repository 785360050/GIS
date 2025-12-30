#pragma once

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <Scene.h>
#include <SceneGraphicsView.h>
#include <Graphic.h>
#include <Ogc3dTilesLayer.h>

//OGC 3D Tiles 规范的 3D 瓦片数据的图层
// https://developers.arcgis.com/qt/cpp/api-reference/esri-arcgisruntime-ogc3dtileslayer.html#details
// https://www.ogc.org/standards/3dtiles/
class Tile_Layer_3D : public QWidget
{
    Q_OBJECT
public slots:

private:
    Esri::ArcGISRuntime::Scene* m_scene = nullptr;
    Esri::ArcGISRuntime::SceneGraphicsView* m_sceneView = nullptr;
    Esri::ArcGISRuntime::Ogc3dTilesLayer* m_ogc3dTilesLayer = nullptr;
public:
    explicit Tile_Layer_3D(QWidget* parent = nullptr);
protected:
    void add3DTilesLayer();



};

