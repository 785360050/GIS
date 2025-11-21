#pragma once

#include <Viewpoint.h>

namespace Esri::ArcGISRuntime {
class Map;
class MapGraphicsView;
} // namespace Esri::ArcGISRuntime

#include <QMainWindow>

class Bookmark_Manage : public QWidget
{
    Q_OBJECT
private:
    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
    // QComboBox* m_bookmarkCombo = nullptr;
    // QPushButton* m_button = nullptr;
    // QInputDialog* m_inputDialog = nullptr;
    QMap<QString, Esri::ArcGISRuntime::Viewpoint> m_bookmarks;
public:
    explicit Bookmark_Manage(QWidget* parent = nullptr);
public:
    void Create_Initial_Bookmarks();
    void Create_Bookmark(QString name, Esri::ArcGISRuntime::Viewpoint viewpoint);
    void Create_Bookmark_At_Current_Viewpoint(QString name);

};

