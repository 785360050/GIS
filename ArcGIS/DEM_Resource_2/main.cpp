// Copyright 2025 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

// Qt headers
#include <QApplication>
#include <QMessageBox>

#include <QSurfaceFormat>

#include "ArcGISRuntimeEnvironment.h"

#include "DEM.h"

using namespace Esri::ArcGISRuntime;

int main(int argc, char *argv[])
{
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    // Linux requires 3.2 OpenGL Context
    // in order to instance 3D symbols
    QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
    fmt.setVersion(3, 2);
    QSurfaceFormat::setDefaultFormat(fmt);
#endif

    QApplication application(argc, argv);
    ArcGISRuntimeEnvironment::setUseLegacyAuthentication(false);
    // Use of ArcGIS location services, such as basemap styles, geocoding, and routing services,
    // requires an access token. For more information see
    // https://links.esri.com/arcgis-runtime-security-auth.

    // The following methods grant an access token:

    // 1. User authentication: Grants a temporary access token associated with a user's ArcGIS account.
    // To generate a token, a user logs in to the app with an ArcGIS account that is part of an
    // organization in ArcGIS Online or ArcGIS Enterprise.

    // 2. API key authentication: Get a long-lived access token that gives your application access to
    // ArcGIS location services. Go to the tutorial at https://links.esri.com/create-an-api-key.
    // Copy the API Key access token.

    const QString accessToken = QString("AAPTxy8BH1VEsoebNVZXo8HurILwSW0Wb_rcoEgSCo68QwpWDdse9wBZ4HpeMmcd-jOq7xZ84Q--jWbMPrie89X-5SdXfoM-ahpWYzeMjb_EHAI6fQ397Y76VDNhsHCqPuBFrlveSiqvlkqurjK8QY8raHuis8R-agQq8e-fZy5kiG7OCMjieshxTxuLcebvPHLu_8WO0QV_oKVfHjmVtEijLyQVSz206_3efIas1pTxopM.AT1_MEG1D55h");

    if (accessToken.isEmpty()) {
        qWarning()
            << "Use of ArcGIS location services, such as the basemap styles service, requires"
            << "you to authenticate with an ArcGIS account or set the API Key property.";
    } else {
        ArcGISRuntimeEnvironment::setApiKey(accessToken);
    }

    // Production deployment of applications built with ArcGIS Maps SDK requires you to
    // license ArcGIS Maps SDK functionality. For more information see
    // https://links.esri.com/arcgis-runtime-license-and-deploy.

    // ArcGISRuntimeEnvironment::setLicense("Place license string in here");

    //  use this code to check for initialization errors
    //  QObject::connect(ArcGISRuntimeEnvironment::instance(), &ArcGISRuntimeEnvironment::errorOccurred, [](const Error& error){
    //    QMessageBox msgBox;
    //    msgBox.setText(error.message);
    //    msgBox.exec();
    //  });

    //  if (ArcGISRuntimeEnvironment::initialize() == false)
    //  {
    //    application.quit();
    //    return 1;
    //  }

    DEM applicationWindow;
    applicationWindow.resize(800,600);
    applicationWindow.show();

    // Shapefile shapefile;
    // shapefile.resize(800,600);
    // shapefile.show();

    // Layer_Mesh_Integrated layer_mesh;
    // layer_mesh.resize(800,600);
    // layer_mesh.show();

    // Tile_Layer_3D tile_layer;
    // tile_layer.resize(800,600);
    // tile_layer.show();

    return application.exec();
}
