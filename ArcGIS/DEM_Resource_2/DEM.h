// Copyright 2025 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#ifndef DEM_H
#define DEM_H

#include "Scene.h"
#include <RasterLayer.h>
#include <Raster.h>
#include <FeatureLayer.h>

#include <QApplication>

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <QMainWindow>

class DEM : public QMainWindow
{
    Q_OBJECT


public:
    explicit DEM(QWidget* parent = nullptr);
    ~DEM(){} ;

private:
    void setupUi();
    void loadDataAndPrintExtent();
private:
    //加载高程数据
    void loadElevation();
    void applyRenderer(Esri::ArcGISRuntime::RasterLayer *layer, Esri::ArcGISRuntime::Raster *raster);
    //使用Esri 提供的全球 OSM 3D 建筑图层
    void useEsriOnlineBuild();
    //使用下载的gpkg文件作为建筑图层
    void useGeoPackageBuild();

    void setCameraExtent(const Esri::ArcGISRuntime::Envelope& ext);
private:
    // Esri::ArcGISRuntime::RasterLayer* m_rasterLayer;
    // Esri::ArcGISRuntime::RasterElevationSource* m_elevationSource;
    Esri::ArcGISRuntime::FeatureLayer* m_buildingLayer; // 建筑要素层
    // Esri::ArcGISRuntime::RasterLayer* m_basemapLayer;
    // Esri::ArcGISRuntime::Camera m_camera;

private:
    Esri::ArcGISRuntime::SceneGraphicsView *m_sceneView = nullptr;
    Esri::ArcGISRuntime::Scene *m_scene;
    QString m_mapTifPath = "../../Resource/Basemap/cliped.tif";
    QString m_elevTifPath = "../../Resource/Elevation/output_hh.tif";
    QString gpkgPath = "../../Resource/Building/test1.gpkg";



};

#endif // DEM_H
