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
};
