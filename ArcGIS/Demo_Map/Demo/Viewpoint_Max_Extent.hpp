#pragma once

#include <Map.h>
#include <MapTypes.h>
#include <Graphic.h>
#include <Envelope.h>
#include <SimpleLineSymbol.h>
#include <MapGraphicsView.h>

#include <QColor>

#include <QMainWindow>

class Viewpoint_Max_Extent : public QWidget
{
    Q_OBJECT
private:
    Esri::ArcGISRuntime::Map* m_map = new Esri::ArcGISRuntime::Map(Esri::ArcGISRuntime::BasemapStyle::ArcGISStreets, this);
    Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;

    Esri::ArcGISRuntime::Graphic* m_coloradoGraphic = nullptr;
    Esri::ArcGISRuntime::GraphicsOverlay* m_coloradoOverlay = nullptr;
    Esri::ArcGISRuntime::Envelope m_coloradoEnvelope;
    Esri::ArcGISRuntime::SimpleLineSymbol* m_coloradoOutline = nullptr;
    bool m_maxExtentEnabled = true;
public:
    explicit Viewpoint_Max_Extent(QWidget* parent = nullptr);
public:
    void Create_Extent_Border();
};

