#include "ArcGIS_Window.hpp"

#include <QTabBar>
#include <QSlider>
#include <QCheckBox>

#include "Utils/Signal_Proxy.hpp"

ArcGIS_Window::ArcGIS_Window(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    this->setWindowTitle("ArcGIS Widget Demo");



    connect(ui.slider_frame,&QSlider::sliderMoved,&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Set_Frame);
    connect(ui.slider_zoom,&QSlider::sliderMoved,&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Set_Zoom);
    connect(ui.slider_speed,&QSlider::sliderMoved,&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Set_Speed);
    connect(ui.slider_angle,&QSlider::sliderMoved,&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Set_Angle);
    connect(ui.mission_select,&QComboBox::currentTextChanged,&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Mission_Selected);
    connect(ui.sync_viewpoint,&QPushButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Enable_Viewpoint_Syncronize);
    connect(ui.button_play,&QPushButton::clicked,&Signal_Proxy::Instance(),[&](bool checked)
    {
        bool status{ui.button_play->isChecked()};
        ui.button_play->setText(status?"Pause":"Play");
        emit Signal_Proxy::Instance().Animate_Play(status);
    });
    connect(ui.camera_follow,&QPushButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Camera_Follow);

    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Animate_UI_Update_Frame_Size,this,[&](int size){ui.slider_frame->setMaximum(size);});
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Animate_UI_Update_Frame_Index,this,[&](int index){ui.value_frame->setText(QString::number(index));ui.slider_frame->setValue(index);});

    // trigger initialize status
    emit Signal_Proxy::Instance().Animate_Mission_Selected(ui.mission_select->currentText());
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Point_Elevation_Show,this,[&](double value_elevation)
    {
        ui.value_elevation->setText(QString::number(value_elevation));
        // ui.button_clear_elevation_point->setChecked(is_visible);
    });
}
