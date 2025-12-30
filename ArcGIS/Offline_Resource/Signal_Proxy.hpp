#pragma once

#include <QObject>

#include <Camera.h>

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
    void Sync_Viewpoint(const Esri::ArcGISRuntime::Camera& camera);


};
