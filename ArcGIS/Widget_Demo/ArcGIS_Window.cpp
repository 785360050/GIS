#include "ArcGIS_Window.hpp"

#include <QTabBar>

ArcGIS_Window::ArcGIS_Window(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    this->setWindowTitle("ArcGIS Widget Demo");

    ui.tabWidget->tabBar()->setDocumentMode(true);
}
