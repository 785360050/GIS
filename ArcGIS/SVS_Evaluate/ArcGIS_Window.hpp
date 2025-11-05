#pragma once

#include "ui_ArcGIS_Window.h"

class ArcGIS_Window : public QWidget
{
    Q_OBJECT
private:
    Ui::ArcGIS_Window ui;
public:
    explicit ArcGIS_Window(QWidget *parent = nullptr);

};
