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

#include <QApplication>

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <QMainWindow>

class DEM : public QMainWindow
{
    Q_OBJECT
public slots:
private:
    Esri::ArcGISRuntime::Scene* m_scene = nullptr;
    Esri::ArcGISRuntime::SceneGraphicsView* m_sceneView = nullptr;

    // QString gpkgPath = "/mnt/Workspace/Experiment/DEM_Resource/Resource/us_building.gpkg";
public:
    explicit DEM(QWidget* parent = nullptr);
    ~DEM() override;
private:

    void loadDataAndPrintExtent();
    void useGeoPackageBuild();

    void Load_Elevation();
    void Load_Basemap();

    void Config_Renderer(Esri::ArcGISRuntime::RasterLayer* layer, Esri::ArcGISRuntime::Raster* raster);


    void Set_Camera(const Esri::ArcGISRuntime::Envelope &ext);
};

#endif // DEM_H
