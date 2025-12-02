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

    ui.tabWidget->tabBar()->setDocumentMode(true);


    connect(ui.draped_visibility,&QCheckBox::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Surface_Placement_Change_Draped_Visibility);
    connect(ui.slider_z,&QSlider::sliderMoved,&Signal_Proxy::Instance(),&Signal_Proxy::Surface_Placement_Change_Z);

    connect(ui.spin_terrain_exaggeration,&QDoubleSpinBox::valueChanged,&Signal_Proxy::Instance(),&Signal_Proxy::Terrain_Exaggeration);

    connect(ui.radio_GlobeCameraController,&QRadioButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Camera_GlobeCameraController);
    connect(ui.radio_OrbitGeoElementCameraController,&QRadioButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Camera_OrbitGeoElementCameraController);
    connect(ui.radio_OrbitLocationCameraController,&QRadioButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Camera_OrbitLocationCameraControlle);

    connect(ui.slider_pitch,&QSlider::sliderMoved,&Signal_Proxy::Instance(),[&](double value){Signal_Proxy::Instance().Scene_Properties_Set_Picth(value);ui.value_pitch->setText(QString::number(value));});
    connect(ui.slider_roll,&QSlider::sliderMoved,&Signal_Proxy::Instance(),[&](double value){Signal_Proxy::Instance().Scene_Properties_Set_Roll(value);ui.value_roll->setText(QString::number(value));});
    connect(ui.slider_heading,&QSlider::sliderMoved,&Signal_Proxy::Instance(),[&](double value){Signal_Proxy::Instance().Scene_Properties_Set_Heading(value);ui.value_heading->setText(QString::number(value));});

    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Point_Elevation_Show,this,[&](double value){ui.value_elevation->setText(QString::number(value));});

    connect(ui.slider_frame,&QSlider::sliderMoved,&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Set_Frame);
    connect(ui.slider_zoom,&QSlider::sliderMoved,&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Set_Zoom);
    connect(ui.slider_speed,&QSlider::sliderMoved,&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Set_Speed);
    connect(ui.slider_angle,&QSlider::sliderMoved,&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Set_Angle);
    connect(ui.mission_select,&QComboBox::currentTextChanged,&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Mission_Selected);
    connect(ui.button_play,&QPushButton::clicked,&Signal_Proxy::Instance(),[&](bool checked)
    {
        bool status{ui.button_play->isChecked()};
        ui.button_play->setText(status?"Pause":"Play");
        emit Signal_Proxy::Instance().Animate_Play(status);
    });
    connect(ui.camera_follow,&QPushButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Animate_Camera_Follow);

    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Animate_UI_Update_Frame_Size,this,[&](int size){ui.slider_frame->setMaximum(size);});

    // trigger initialize status
    Signal_Proxy::Instance().Animate_Mission_Selected(ui.mission_select->currentText());
}
