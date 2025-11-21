#pragma once

#include <QObject>
#include <QAbstractListModel>

#include "MapTypes.h"

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
    void Dispaly_Map_Status_Update(Esri::ArcGISRuntime::LoadStatus);

    void Basemap_Change(int index);

    void Existing_Map_Open(const QString& map);

    void Viewpoint_Change(int index);
    // void Viewpoint_Rotate(int index);
    // void Viewpoint_();
    void Viewpoint_Center();
    void Viewpoint_Center_scale();
    void Viewpoint_Geometry();
    void Viewpoint_Geometry_Padding();
    void Viewpoint_Rotation();
    void Viewpoint_Scale(int value);
    void Viewpoint_Scale_AnimationCurve(int duration,int animate_type);

    void Bookmark_Create(QString name);
    void Bookmark_List_Update(std::vector<QString> bookmark_list);
    void Bookmark_Select_Bookmark(QString bookmark);

    void Basemap_Language_Global(bool is_global);
    void Basemap_Language_Select(QString language);

    void Reference_Scale_Select(int scale_value);
    void Reference_Scale_Syncronize();
    void Reference_Scale_Update_Operationl_Layers(QAbstractListModel* list);
    void Reference_Scale_Feature_Layer(const QString &layerName, bool checkedStatus);
};
