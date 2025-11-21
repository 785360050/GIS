#pragma once

#include <Map.h>
#include <MapTypes.h>
#include <Portal.h>
#include <PortalItem.h>
#include <MapGraphicsView.h>

#include <QAbstractListModel>

#include <QFuture>

class Map_Reference_Scale : public QWidget
{
    Q_OBJECT
private:
    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
    Esri::ArcGISRuntime::Portal* m_portal = new Esri::ArcGISRuntime::Portal(this);
    Esri::ArcGISRuntime::PortalItem* m_portalItem = new Esri::ArcGISRuntime::PortalItem(m_portal, "3953413f3bd34e53a42bf70f2937a408", this);
    QAbstractListModel* m_layerInfoListModel = nullptr;
public:
    explicit Map_Reference_Scale(QWidget* parent = nullptr);
public:
    void setCurrentMapScale(double scale)
    {
        m_map->setReferenceScale(scale);
    }

    void setMapScaleToReferenceScale()
    {
        if(m_mapView)
            m_mapView->setViewpointScaleAsync(m_map->referenceScale());
    }

    void featureLayerScaleSymbols(const QString& layerName, bool checkedStatus);

};

