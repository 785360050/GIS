#include "Bookmark_Manage.hpp"

#include <QVBoxLayout>
#include <QFuture>

#include <Map.h>
#include <Bookmark.h>
#include <BookmarkListModel.h>
#include <MapGraphicsView.h>
#include <MapTypes.h>
#include <Envelope.h>
#include <Viewpoint.h>
#include <SpatialReference.h>

#include "../Utils/Signal_Proxy.hpp"

using namespace Esri::ArcGISRuntime;

Bookmark_Manage::Bookmark_Manage(QWidget* parent /*=nullptr*/)
    : QWidget(parent)
{
    // Create a map using the imagery with labels basemap
    m_map = new Map(BasemapStyle::ArcGISImagery, this);
    Envelope env1(3742993.127298778, 3170396.4675719286, 3744795.1333054285, 3171745.88077, SpatialReference(102100));
    m_map->setInitialViewpoint(Viewpoint(env1));

    // Create a map view, and pass in the map
    m_mapView = new MapGraphicsView(m_map, this);

    // Create the initial bookmarks
    Create_Initial_Bookmarks();

    // // Connect the combo box signal to lambda for switching between bookmarks
    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Bookmark_Select_Bookmark, [this](const QString& name)
    {
        // Use the map of bookmarks created to obtain the viewpoint of a given name
        m_mapView->setViewpointAsync(m_bookmarks.value(name));
    });

    // // Connect the button clicked signal to lambda for showing input dialog
    // connect(m_button, &QPushButton::clicked, [this]() {
    //     m_inputDialog->show();
    // });

    // // Connect the input dialog for newly added bookmarks
    // connect(m_inputDialog, &QInputDialog::accepted, [this]() {
    //     createBookmark(m_inputDialog->textValue(), m_mapView->currentViewpoint(ViewpointType::BoundingGeometry));
    // });

    connect(&Signal_Proxy::Instance(),&Signal_Proxy::Bookmark_Create,this,&Bookmark_Manage::Create_Bookmark_At_Current_Viewpoint);

    // Set up the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

void Bookmark_Manage::Create_Initial_Bookmarks()
{
    // Mysterious Desert Pattern
    Envelope env1(3742993.127298778, 3170396.4675719286, 3744795.1333054285, 3171745.88077, SpatialReference(102100));
    Viewpoint viewpoint1(env1);
    Create_Bookmark("Mysterious Desert Pattern", viewpoint1);

    // Strange Symbol
    Envelope env2(-13009913.860076642, 4495026.9307899885, -13009442.089218518, 4495404.031910696, SpatialReference(102100));
    Viewpoint viewpoint2(env2);
    Create_Bookmark("Strange Symbol", viewpoint2);

    // Guitar-Shaped Forest
    Envelope env3(-7124497.45137465, -4012221.6124684606, -7121131.417429369, -4009697.0870095, SpatialReference(102100));
    Viewpoint viewpoint3(env3);
    Create_Bookmark("Guitar-Shaped Forest", viewpoint3);

    // Grand Prismatic Spring
    Envelope env4(-12338668.348591767, 5546908.424239618, -12338247.594362013, 5547223.989911933, SpatialReference(102100));
    Viewpoint viewpoint4(env4);
    Create_Bookmark("Grand Prismatic Spring", viewpoint4);
}

void Bookmark_Manage::Create_Bookmark_At_Current_Viewpoint(QString name)
{
    auto viewpoint=m_mapView->currentViewpoint(ViewpointType::BoundingGeometry);

    Create_Bookmark(name,viewpoint);
}
void Bookmark_Manage::Create_Bookmark(QString name, Esri::ArcGISRuntime::Viewpoint viewpoint)
{
    // Create the bookmark from the name and viewpoint
    Bookmark* bookmark = new Bookmark(name, viewpoint, this);

    // Add it to the map's bookmark list
    m_map->bookmarks()->append(bookmark);

    // Add it to a bookmark map so it can be accessed later

    m_bookmarks.insert(name,viewpoint);

    std::vector<QString> bookmark_list;
    for(const auto& bookmark_name:m_bookmarks.keys())
        bookmark_list.push_back(bookmark_name);
    Signal_Proxy::Instance().Bookmark_List_Update(bookmark_list);
}

