#include "Main_Window.hpp"

#include "MapTypes.h"

#include "Utils/Signal_Proxy.hpp"

Main_Window::Main_Window(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Dispaly_Map_Status_Update, this, [this](Esri::ArcGISRuntime::LoadStatus loadStatus)
            {
                switch (loadStatus)
                {
                case Esri::ArcGISRuntime::LoadStatus::Loaded:
                    ui.map_load_status->setText(QString("Map Load Status: Loaded"));
                    break;
                case Esri::ArcGISRuntime::LoadStatus::Loading:
                    ui.map_load_status->setText(QString("Map Load Status: Loading..."));
                    break;
                case Esri::ArcGISRuntime::LoadStatus::FailedToLoad:
                    ui.map_load_status->setText(QString("Map Load Status: Failed to Load"));
                    break;
                case Esri::ArcGISRuntime::LoadStatus::NotLoaded:
                    ui.map_load_status->setText(QString("Map Load Status: Not Loaded"));
                    break;
                case Esri::ArcGISRuntime::LoadStatus::Unknown:
                    ui.map_load_status->setText(QString("Map Load Status: Unknown"));
                    break;
                }
            });

    connect(ui.basemap_change,&QComboBox::currentIndexChanged,&Signal_Proxy::Instance(),&Signal_Proxy::Basemap_Change);

    // connect(ui.map_select,&QComboBox::currentTextChanged,&Signal_Proxy::Instance(),&Signal_Proxy::Existing_Map_Select);
    connect(ui.button_map_open,&QPushButton::clicked,&Signal_Proxy::Instance(),[&](){Signal_Proxy::Instance().Existing_Map_Open(ui.map_select->currentText());});

    connect(ui.viewpoint_change,&QComboBox::currentIndexChanged,&Signal_Proxy::Instance(),&Signal_Proxy::Viewpoint_Change);
    connect(ui.viewpoint_change_rotate,&QPushButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Viewpoint_Rotation);

    connect(ui.viewpoint_change_center,&QPushButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Viewpoint_Center);
    connect(ui.viewpoint_change_center_scale,&QPushButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Viewpoint_Center_scale);
    connect(ui.viewpoint_change_geometry,&QPushButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Viewpoint_Geometry);
    connect(ui.viewpoint_change_geometry_padding,&QPushButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Viewpoint_Geometry_Padding);
    connect(ui.viewpoint_change_scale,&QPushButton::clicked,&Signal_Proxy::Instance(),[&](){Signal_Proxy::Instance().Viewpoint_Scale(ui.viewpoint_scale_value->text().toDouble());});
    connect(ui.viewpoint_change_scale_animationCurve,&QPushButton::clicked,&Signal_Proxy::Instance(),[&]()
    {
        Signal_Proxy::Instance().Viewpoint_Scale_AnimationCurve(ui.viewpoint_animate_duration->text().toInt(),ui.viewpoint_animate_type->currentIndex());
    });

    // connect(ui.button_create_bookmark,&QPushButton::clicked,&Signal_Proxy::Instance(),[&](QString name, Esri::ArcGISRuntime::Viewpoint viewpoint)
    connect(ui.button_create_bookmark,&QPushButton::clicked,&Signal_Proxy::Instance(),[&]()
    {
        static int id{};
        Signal_Proxy::Instance().Bookmark_Create(QString("created %1").arg(id++));
    });
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Bookmark_List_Update,this,&Main_Window::Update_Bookmark_List);
    connect(ui.list_bookmark, &QListWidget::itemDoubleClicked,this, [&](QListWidgetItem* item){Signal_Proxy::Instance().Bookmark_Select_Bookmark(item->text());});

}

void Main_Window::Update_Bookmark_List(std::vector<QString> bookmark_list)
{
    // 清空原有内容
    ui.list_bookmark->clear();

    // 写入新的书签列表
    for (const QString& name : bookmark_list)
        ui.list_bookmark->addItem(name);
}
