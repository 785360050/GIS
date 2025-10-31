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

class MissionData
{
public:

    struct DataPoint
    {
        DataPoint(){}
        DataPoint(double lon, double lat, double elevation, double heading, double pitch, double roll):
            m_pos(Esri::ArcGISRuntime::Point(lon, lat, elevation, Esri::ArcGISRuntime::SpatialReference::wgs84())),
            m_heading(heading),
            m_pitch(pitch),
            m_roll(roll){}

        Esri::ArcGISRuntime::Point m_pos;
        double m_heading = NAN;
        double m_pitch = NAN;
        double m_roll = NAN;
    };

    typedef std::vector<DataPoint> DataPointList;

    MissionData()=default;
    ~MissionData()=default;

    bool parse(const QString& dataPath)
    {
        m_data.clear();
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

            m_data.emplace_back((double)lon, (double)lat, (double)elevation, (double)heading, (double)pitch, (double)roll);
        }

        m_ready = m_data.size() > 0;
        return m_ready;
    }
    bool isEmpty() const {return m_data.empty();}
    size_t size() const {return m_data.size();}
    const DataPoint& dataAt(size_t i) const
    {
        if(i < m_data.size())
            return m_data[i];

        static MissionData::DataPoint dataPoint;
        return dataPoint;
    }
    bool ready() const {return m_ready;}

private:
    DataPointList m_data;
    bool m_ready{false};
};

class Animate_Symbol : public QWidget
{
    Q_OBJECT
public slots:

private:
    Esri::ArcGISRuntime::Scene* m_scene = nullptr;
    Esri::ArcGISRuntime::SceneGraphicsView* m_sceneView = nullptr;
    Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
    Esri::ArcGISRuntime::ModelSceneSymbol* m_model3d = nullptr;
    Esri::ArcGISRuntime::Graphic* m_graphic3d = nullptr;
    Esri::ArcGISRuntime::Graphic* m_graphic2d = nullptr;
    Esri::ArcGISRuntime::SimpleMarkerSymbol* m_symbol2d = nullptr;
    Esri::ArcGISRuntime::Graphic* m_routeGraphic = nullptr;
    Esri::ArcGISRuntime::GlobeCameraController* m_globeController = nullptr;
    Esri::ArcGISRuntime::OrbitGeoElementCameraController* m_followingController = nullptr;
    QString m_dataPath;
    QAbstractListModel* m_missionsModel = nullptr;
    std::unique_ptr<MissionData> m_missionData;
    int m_frame = 0;
    double m_mapZoomFactor = 5.0;
    double m_animationSpeed = 100.0; // 默认速度（0–100%）
    QTimer* timer{};
public:
    explicit Animate_Symbol(QWidget* parent = nullptr);

private:
    void _Signal_Bind();
private:
    int missionFrame() const{return m_frame;}
    bool missionReady() const
    {
        if (!m_missionData)
            return false;

        return m_missionData->ready();
    }
    void setMissionFrame(int newFrame)
    {
        if (!m_missionData||
            newFrame < 0 ||
            m_frame == newFrame)
            return;

        m_frame = newFrame;
        // emit missionFrameChanged();
    }
    void changeMission(const QString &missionNameStr);
    int missionSize() const
    {
        if (!m_missionData)
            return 0;

        return (int)m_missionData->size();
    }
    void setFollowing(bool following)
    {
        if (following)
            m_sceneView->setCameraController(m_followingController);
        else
            m_sceneView->setCameraController(m_globeController);
    }

    void animate();

    void setAnimationSpeed(double value)
    {
        m_animationSpeed = std::clamp(value, 0.0, 100.0);
        updateTimerInterval();
    }
    void updateTimerInterval();

    void setZoom(double zoomDist)
    {
        if (m_followingController)
        {
            m_followingController->setCameraDistance(zoomDist);
            // emit zoomChanged();
        }
    }

    void setAngle(double angle)
    {
        if (m_followingController)
        {
            m_followingController->setCameraPitchOffset(angle);
            // emit angleChanged();
        }
    }


public:
    void createRoute2d(Esri::ArcGISRuntime::GraphicsOverlay* mapOverlay);

    void createModel2d(Esri::ArcGISRuntime::GraphicsOverlay* mapOverlay);

    void createGraphic3D();

};

