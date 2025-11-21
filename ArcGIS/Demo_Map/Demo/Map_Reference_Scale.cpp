#include "Map_Reference_Scale.hpp"

#include <QVBoxLayout>

#include <Map.h>
#include <MapGraphicsView.h>
#include <MapTypes.h>
#include <Error.h>
#include <LayerListModel.h>
#include <FeatureLayer.h>

#include "../Utils/Signal_Proxy.hpp"

using namespace Esri::ArcGISRuntime;

Map_Reference_Scale::Map_Reference_Scale(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a map using the ArcGISStreets BasemapStyle
    // m_map = new Map(BasemapStyle::ArcGISStreets, this);
    m_map = new Map(m_portalItem, this);

    // Create the Widget view
    m_mapView = new MapGraphicsView(this);

    // Set map to map view
    m_mapView->setMap(m_map);

    // Once map is loaded set FeatureLayer list model
    connect(m_map, &Map::doneLoading, this, [this](const Error& loadError)
    {
        if (!loadError.isEmpty())
            return;

        m_layerInfoListModel = m_map->operationalLayers();
        // emit layerInfoListModelChanged();
        // emit currentMapScaleChanged();

        Signal_Proxy::Instance().Reference_Scale_Update_Operationl_Layers(m_layerInfoListModel);

        connect(m_mapView, &MapGraphicsView::mapScaleChanged, this, [this]()
                {
                    // emit currentMapScaleChanged();
                });
    });

    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Reference_Scale_Select,this,&Map_Reference_Scale::setCurrentMapScale);
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Reference_Scale_Syncronize,this,&Map_Reference_Scale::setMapScaleToReferenceScale);
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Reference_Scale_Feature_Layer,this,&Map_Reference_Scale::featureLayerScaleSymbols);


    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

void Map_Reference_Scale::featureLayerScaleSymbols(const QString &layerName, bool checkedStatus)
{
    if(m_layerInfoListModel)
    {
        for(Layer* layer : *static_cast<LayerListModel*>(m_layerInfoListModel))
        {
            if(layer->name() == layerName)
            {
                FeatureLayer* featureLayer = static_cast<FeatureLayer*>(layer);
                if(featureLayer)
                    featureLayer->setScaleSymbols(checkedStatus);
            }
        }
    }
}


