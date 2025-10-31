#pragma once

namespace Esri::ArcGISRuntime {
class SceneGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <Scene.h>
#include <SceneGraphicsView.h>
#include <Graphic.h>
#include <MapGraphicsView.h>
#include <ModelSceneSymbol.h>
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
#include <QAbstractListModel>

class Mission_Data
{
public:

    struct Point
    {
        Point(){}
        Point(double lon, double lat, double elevation, double heading, double pitch, double roll):
            m_pos(Esri::ArcGISRuntime::Point(lon, lat, elevation, Esri::ArcGISRuntime::SpatialReference::wgs84())),
            m_heading(heading),
            m_pitch(pitch),
            m_roll(roll){}

        Esri::ArcGISRuntime::Point m_pos;
        double m_heading = NAN;
        double m_pitch = NAN;
        double m_roll = NAN;
    };
private:
    std::vector<Point> point;
    bool m_ready{false};

public:
    Mission_Data()=default;
    ~Mission_Data()=default;

public:
    bool parse(const QString& dataPath)
    {
        point.clear();
        m_ready = false;

        QFile file(dataPath);
        if(!file.exists())
            return false;

        if (!file.open(QIODevice::ReadOnly))
            return false;

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

        m_ready = point.size() > 0;
        return m_ready;
    }
    bool isEmpty() const {return point.empty();}
    size_t size() const {return point.size();}
    const Point& operator[](size_t index) const
    {
        if(index < point.size())
            return point[index];

        static Mission_Data::Point dataPoint;
        return dataPoint;
    }
    bool ready() const {return m_ready;}


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
    Esri::ArcGISRuntime::ModelSceneSymbol* model_3d = nullptr;
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
private:
    int missionFrame() const{return frame_index;}
    bool missionReady() const
    {
        return mission_data.ready();
    }
    void setMissionFrame(int newFrame)
    {
        if (newFrame < 0 || frame_index == newFrame)
            return;

        frame_index = newFrame;
        // emit missionFrameChanged();
    }
    void changeMission(const QString &missionNameStr);
    int missionSize() const
    {
        return (int)mission_data.size();
    }
    void setFollowing(bool following)
    {
        if (following)
            scene_view->setCameraController(camera_controller_OrbitGeoElement);
        else
            scene_view->setCameraController(camera_controller_globe);
    }

    void animate();

    void setAnimationSpeed(double value)
    {
        animation_speed = std::clamp(value, 0.0, 100.0);
        updateTimerInterval();
    }
    void updateTimerInterval();

    void setZoom(double zoomDist)
    {
        if (camera_controller_OrbitGeoElement)
        {
            camera_controller_OrbitGeoElement->setCameraDistance(zoomDist);
            // emit zoomChanged();
        }
    }

    void setAngle(double angle)
    {
        if (camera_controller_OrbitGeoElement)
        {
            camera_controller_OrbitGeoElement->setCameraPitchOffset(angle);
            // emit angleChanged();
        }
    }


public:
    void createRoute2d(Esri::ArcGISRuntime::GraphicsOverlay* mapOverlay);

    void createModel2d(Esri::ArcGISRuntime::GraphicsOverlay* mapOverlay);

    void createGraphic3D();

};

