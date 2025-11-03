#pragma once

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <Scene.h>
#include <SceneGraphicsView.h>
#include <Graphic.h>
#include <MapGraphicsView.h>
#include <ModelSceneSymbol.h>
#include <DistanceCompositeSceneSymbol.h>
#include <GraphicsOverlay.h>
#include <SimpleMarkerSymbol.h>
#include <GlobeCameraController.h>
#include <OrbitGeoElementCameraController.h>


// ArcGIS Maps SDK headers
#include "Point.h"
#include "SpatialReference.h"

// STL headers
#include <cmath>
#include <QFile>
#include <QFuture>
#include <QMouseEvent>
#include <QAbstractListModel>

class Mission_Data
{
public:

    struct Point
    {
        Point(){}
        Point(double lon, double lat, double elevation, double heading, double pitch, double roll):
            pos(Esri::ArcGISRuntime::Point(lon, lat, elevation, Esri::ArcGISRuntime::SpatialReference::wgs84())),
            heading(heading),
            pitch(pitch),
            roll(roll){}

        Esri::ArcGISRuntime::Point pos;
        double heading{};
        double pitch{};
        double roll{};
    };
// private:
public:
    std::vector<Point> point;
    bool ready{false};

public:
    Mission_Data()=default;
    ~Mission_Data()=default;

public:
    void Load_File(const QString& file_path)
    {
        point.clear();
        ready = false;

        QFile file(file_path);
        if(!file.exists())
            throw std::runtime_error("file not exists");

        if (!file.open(QIODevice::ReadOnly))
            throw std::runtime_error("file open failed");

        while (!file.atEnd())
        {
            QByteArray line = file.readLine();
            QList<QByteArray> parts = line.split(',');
            if(parts.size() < 6)
                continue;

            bool ok = false;
            double lon = parts.at(0).toDouble(&ok);
            if(!ok)
                continue;

            double lat = parts.at(1).toDouble(&ok);
            if(!ok)
                continue;

            double elevation = parts.at(2).toDouble(&ok);
            if(!ok)
                continue;

            double heading = parts.at(3).toDouble(&ok);
            if(!ok)
                continue;

            double pitch = parts.at(4).toDouble(&ok);
            if(!ok)
                continue;

            double roll = parts.at(5).simplified().toDouble(&ok);
            if(!ok)
                continue;

            point.emplace_back((double)lon, (double)lat, (double)elevation, (double)heading, (double)pitch, (double)roll);
        }

        // qDebug()<<"mission point.size() = "<<point.size();
        ready = point.size() > 0;
    }

};

class Animate_Symbol : public QWidget
{
    Q_OBJECT
public slots:

private:
    Mission_Data mission_data;
    int frame_index{};
    double animation_speed{100.0}; // 默认速度（0–100%）
    QTimer* timer{};

private:
    Esri::ArcGISRuntime::Scene* scene = nullptr;
    Esri::ArcGISRuntime::SceneGraphicsView* scene_view = nullptr;
    Esri::ArcGISRuntime::MapGraphicsView* map_view = nullptr;
    // Esri::ArcGISRuntime::ModelSceneSymbol* model_3d = nullptr;
    Esri::ArcGISRuntime::DistanceCompositeSceneSymbol* model_3d = nullptr;
    Esri::ArcGISRuntime::Graphic* graphic_3d = nullptr;
    Esri::ArcGISRuntime::Graphic* graphic_2d = nullptr;
    Esri::ArcGISRuntime::SimpleMarkerSymbol* symbol_2d = nullptr;
    Esri::ArcGISRuntime::Graphic* graphic_route = nullptr;
    Esri::ArcGISRuntime::GlobeCameraController* camera_controller_globe = nullptr;
    Esri::ArcGISRuntime::OrbitGeoElementCameraController* camera_controller_OrbitGeoElement = nullptr;

public:
    explicit Animate_Symbol(QWidget* parent = nullptr);

private:
    void _Signal_Bind();
    void _Syncronize_Viewpoint_To_Map_View();
    void _Syncronize_Viewpoint_To_Scene_View();


private:
    void _Initialize_Map_View(Esri::ArcGISRuntime::GraphicsOverlay* mapOverlay);

    void _Initialize_Scene_View();

    void Set_Mission_Frame(int frame)
    {
        if (frame < 0 or frame>=mission_data.point.size())
            return;

        frame_index = frame;
    }
    void Mission_Change(const QString &missionNameStr);

    void Set_Camera_Following(bool following)
    {
        if (following)
            scene_view->setCameraController(camera_controller_OrbitGeoElement);
        else
            scene_view->setCameraController(camera_controller_globe);
    }

    void _Update_Scene(const Mission_Data::Point &data_point);
    void _Update_Map(const Mission_Data::Point &data_point);
    void Frame_Update();

    void Update_Animate_Speed(double value);

    void Enable_Viewpoint_Syncronize(bool enable);



public:
    QFuture<double> m_elevationQueryFuture;
    Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
    Esri::ArcGISRuntime::Graphic* m_elevationMarker = nullptr;
    void displayElevationOnClick(QMouseEvent &mouseEvent);

};

