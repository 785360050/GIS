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
#include <GeoPackage.h>

#include <QApplication>

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <QMainWindow>

class DEM : public QMainWindow
{
    Q_OBJECT

private:
    Esri::ArcGISRuntime::SceneGraphicsView *m_sceneView = nullptr;
    Esri::ArcGISRuntime::Scene *m_scene;


private:

    // Esri::ArcGISRuntime::RasterLayer* m_rasterLayer;
    // Esri::ArcGISRuntime::RasterElevationSource* m_elevationSource;
    Esri::ArcGISRuntime::FeatureLayer* m_buildingLayer; // 建筑要素层
    Esri::ArcGISRuntime::RasterLayer* mapLayer;
    // Esri::ArcGISRuntime::RasterLayer* m_basemapLayer;
    // Esri::ArcGISRuntime::Camera m_camera;
public:
    explicit DEM(QWidget* parent = nullptr);
    ~DEM(){} ;

private:
    //加载高程数据
    void _Load_Elevation();
    // void applyRenderer(Esri::ArcGISRuntime::RasterLayer *layer, Esri::ArcGISRuntime::Raster *raster);
    //使用下载的gpkg文件作为建筑图层
    void _Load_GeoPackage();
    void _Load_Basemap();
    void Load_Resource();

    void Set_Camera_Extent(const Esri::ArcGISRuntime::Envelope& ext);

private:
    void Handle_MapLayer();
    void Handle_Building(Esri::ArcGISRuntime::GeoPackage* gpkg);
};

#endif // DEM_H
