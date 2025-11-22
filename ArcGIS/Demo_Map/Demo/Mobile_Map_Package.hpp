#pragma once

#include <Map.h>
#include <MapTypes.h>
#include <MapGraphicsView.h>
#include <MobileMapPackage.h>
#include <Error.h>

#include <QWidget>

class Mobile_Map_Package : public QWidget
{
    Q_OBJECT
private:
    // Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
    Esri::ArcGISRuntime::MobileMapPackage* m_mobileMapPackage = nullptr;
public:
    explicit Mobile_Map_Package(QWidget* parent = nullptr);
protected:
    void Load_Map(const Esri::ArcGISRuntime::Error& error);
};

