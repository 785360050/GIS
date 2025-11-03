#pragma once

#include <QObject>


class Signal_Proxy:public QObject
{
    Q_OBJECT
private: //仅允许内部构造
    // explicit Signal_Proxy(QObject *parent = nullptr);
    Signal_Proxy()=default;
    ~Signal_Proxy() = default;

public: // 单例阻止拷贝构造和拷贝赋值
    Signal_Proxy(const Signal_Proxy &) = delete;
    Signal_Proxy &operator= (const Signal_Proxy &) = delete;

public: 
    static Signal_Proxy &Instance()
    { // 局部静态变量，线程安全
        static Signal_Proxy instance;  
        return instance;
    };

signals:
    void Surface_Placement_Change_Draped_Visibility();
    void Surface_Placement_Change_Z(int value);

    void Camera_GlobeCameraController();
    void Camera_OrbitGeoElementCameraController();
    void Camera_OrbitLocationCameraControlle();

    void Scene_Properties_Set_Picth(double value);
    void Scene_Properties_Set_Roll(double value);
    void Scene_Properties_Set_Heading(double value);

    void Point_Elevation_Show(double value);

    void Animate_Mission_Selected(const QString mission_name);
    void Animate_Camera_Follow(bool is_follow);
    void Animate_Play(bool is_playing);
    void Animate_Set_Zoom(double value);
    void Animate_Set_Speed(double value);
    void Animate_Set_Angle(double value);
    void Animate_Set_Frame(double value);
    void Animate_UI_Update_Frame_Size(int size);
    void Animate_UI_Update_Frame_Index(int index);
    void Enable_Viewpoint_Syncronize(bool enable);
};
