#include "Identify_Layers.hpp"

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QUrl>
#include <QFuture>
#include <QMessageBox>

#include <ArcGISMapImageLayer.h>
#include <ArcGISSublayer.h>
#include <ArcGISSublayerListModel.h>
#include <Basemap.h>
#include <Error.h>
#include <FeatureLayer.h>
#include <IdentifyLayerResult.h>
#include <LayerListModel.h>
#include <Map.h>
#include <MapTypes.h>
#include <Point.h>
#include <ServiceFeatureTable.h>
#include <SpatialReference.h>
#include <Viewpoint.h>

using namespace Esri::ArcGISRuntime;


static QString m_message;
void Handle_Mouse_Event(const QList<IdentifyLayerResult*>& results)
{
    // reset the message text
    m_message = QString();

    for (IdentifyLayerResult* result : results)
    {
        // lambda for calculating result count
        auto geoElementsCountFromResult = [] (IdentifyLayerResult* result) -> int
        {
            // create temp list
            QList<IdentifyLayerResult*> tempResults{result};

            // use Depth First Search approach to handle recursion
            int count = 0;
            int index = 0;

            while (index < tempResults.length())
            {
                //get the result object from the array
                IdentifyLayerResult* identifyResult = tempResults[index];

                // update count with geoElements from the result
                count += identifyResult->geoElements().length();

                // check if the result has any sublayer results
                // if yes then add those result objects in the tempResults
                // array after the current result
                if (identifyResult->sublayerResults().length() > 0)
                    tempResults.append(identifyResult->sublayerResults().at(index));

                // update the count and repeat
                index += 1;
            }
            return count;
        };

        const int count = geoElementsCountFromResult(result);
        QString layerName = result->layerContent()->name();
        m_message += QString("%1 : %2").arg(layerName).arg(count);
        m_message += "\n";
    }
    //remove last newline character
    if (!m_message.isEmpty())
        m_message.chop(1);

    QMessageBox::warning(nullptr,"Message",m_message);
    qDeleteAll(results);
}



Identify_Layers::Identify_Layers(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a map using the ArcGISStreets BasemapStyle
    m_map = new Map(BasemapStyle::ArcGISTopographic, this);

    // Create the Widget view
    m_mapView = new MapGraphicsView(this);

    // add a map image layer
    QUrl mapServiceUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/SampleWorldCities/MapServer");
    ArcGISMapImageLayer* imageLayer = new ArcGISMapImageLayer(mapServiceUrl, this);
    m_map->operationalLayers()->append(imageLayer);

    // hide 2 of the sublayers in the map image layer
    connect(imageLayer, &ArcGISMapImageLayer::doneLoading, this, [imageLayer](const Error& e)
            {
                if (!e.isEmpty())
                {
                    qDebug() << e.message() << e.additionalMessage();
                    return;
                }

                imageLayer->mapImageSublayers()->at(1)->setVisible(false);
                imageLayer->mapImageSublayers()->at(2)->setVisible(false);
            });

    // add a feature layer
    QUrl featureLayerUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0");
    ServiceFeatureTable* featureTable = new ServiceFeatureTable(featureLayerUrl, this);
    FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);
    m_map->operationalLayers()->append(featureLayer);

    // set an initial viewpoint
    const double mapScale = 68015210;
    const double x = -10977012.785807;
    const double y = 4514257.550369;
    const Point pt(x, y, SpatialReference::webMercator());
    Viewpoint initialViewpoint(pt, mapScale);
    m_map->setInitialViewpoint(initialViewpoint);

    // Set map to map view
    m_mapView->setMap(m_map);

    // identify layers on mouse click
    connect(m_mapView, &MapGraphicsView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
            {
                const double tolerance = 12.0;
                const bool returnPopups = false;
                const int maxResults = 10;

                m_mapView->identifyLayersAsync(mouseEvent.position(), tolerance, returnPopups, maxResults).then(this,Handle_Mouse_Event);

            });

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}


