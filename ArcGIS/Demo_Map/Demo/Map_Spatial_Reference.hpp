#pragma once

#include <Map.h>
#include <Basemap.h>
#include <MapGraphicsView.h>
#include <ArcGISMapImageLayer.h>

#include <QMainWindow>

class Map_Spatial_Reference : public QWidget
{
    Q_OBJECT
public:
    explicit Map_Spatial_Reference(QWidget* parent = nullptr);

};

