#include "Main_Window.hpp"

#include "MapTypes.h"

#include "Utils/Signal_Proxy.hpp"

Main_Window::Main_Window(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // { // multiline tab
    //     QTabBar* bar = ui.tabWidget->tabBar();

    //     // 关闭滚动按钮
    //     bar->setUsesScrollButtons(false);

    //     // 允许换行（关键）
    //     bar->setExpanding(false);   // 不自动扩展到全宽
    //     bar->setElideMode(Qt::ElideNone);

    //     // 在某些平台下需要
    //     bar->setMovable(true);

    //     // 启用多行（Qt 隐藏 API，但有效）
    //     bar->setProperty("documentMode", false);
    //     bar->setStyleSheet("QTabBar::tab { height: 26px; }");
    // }

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

    connect(ui.basemap_language_select,&QComboBox::currentTextChanged,&Signal_Proxy::Instance(),&Signal_Proxy::Basemap_Language_Select);
    connect(ui.basemap_language_is_global,&QPushButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Basemap_Language_Global);

    connect(ui.reference_scale_select,&QComboBox::currentTextChanged,&Signal_Proxy::Instance(),[&](QString text){Signal_Proxy::Instance().Reference_Scale_Select(text.toInt());});
    connect(ui.button_reference_scale_syncronize,&QPushButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Reference_Scale_Syncronize);
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Reference_Scale_Update_Operationl_Layers,this,[&](QAbstractListModel* list)
    {
        // 清空 QListWidget
        ui.reference_operational_layers_list->clear();

        // 遍历 QAbstractListModel
        for (int i = 0; i < list->rowCount(); ++i)
        {
            QModelIndex index = list->index(i, 0);

            // 获取 role 数据，一般 ArcGIS layer list 用 role "name"
            QString name = list->data(index, Qt::DisplayRole).toString();
            if (name.isEmpty())
                name = list->data(index, list->roleNames().key("name")).toString();

            QListWidgetItem* item = new QListWidgetItem(name);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item->setCheckState(Qt::Checked);
            ui.reference_operational_layers_list->addItem(item);
        }

        for (int i = 0; i < list->rowCount(); ++i)
            ui.reference_operational_layers_list->item(i)->setSelected(true);
    });
    ui.reference_operational_layers_list->setSelectionMode(QAbstractItemView::NoSelection);
    connect(ui.reference_operational_layers_list,&QListWidget::itemDoubleClicked,this,[this](QListWidgetItem* item)
    {
        // 切换复选框状态
        Qt::CheckState newState =
            (item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);

        item->setCheckState(newState);

        // 触发自定义 toggled 信号
        // emit itemToggled(item->text(), newState == Qt::Checked);
        Signal_Proxy::Instance().Reference_Scale_Feature_Layer(item->text(),(bool)newState);
    });
}

void Main_Window::Update_Bookmark_List(std::vector<QString> bookmark_list)
{
    // 清空原有内容
    ui.list_bookmark->clear();

    // 写入新的书签列表
    for (const QString& name : bookmark_list)
        ui.list_bookmark->addItem(name);
}
