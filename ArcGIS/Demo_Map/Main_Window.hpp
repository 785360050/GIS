#pragma once

#include "ui_Main_Window.h"

class Main_Window : public QMainWindow
{
    Q_OBJECT
private:
    Ui::Main_Window ui;
public:
    explicit Main_Window(QWidget *parent = nullptr);
protected:
    void Update_Bookmark_List(std::vector<QString> bookmark_list);

};
