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

    connect(ui.radio_GlobeCameraController,&QRadioButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Camera_GlobeCameraController);
    connect(ui.radio_OrbitGeoElementCameraController,&QRadioButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Camera_OrbitGeoElementCameraController);
    connect(ui.radio_OrbitLocationCameraController,&QRadioButton::clicked,&Signal_Proxy::Instance(),&Signal_Proxy::Camera_OrbitLocationCameraControlle);

    connect(ui.slider_pitch,&QSlider::sliderMoved,&Signal_Proxy::Instance(),[&](double value){Signal_Proxy::Instance().Scene_Properties_Set_Picth(value);ui.value_pitch->setText(QString::number(value));});
    connect(ui.slider_roll,&QSlider::sliderMoved,&Signal_Proxy::Instance(),[&](double value){Signal_Proxy::Instance().Scene_Properties_Set_Roll(value);ui.value_roll->setText(QString::number(value));});
    connect(ui.slider_heading,&QSlider::sliderMoved,&Signal_Proxy::Instance(),[&](double value){Signal_Proxy::Instance().Scene_Properties_Set_Heading(value);ui.value_heading->setText(QString::number(value));});

    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Point_Elevation_Show,this,[&](double value){ui.value_elevation->setText(QString::number(value));});
}
