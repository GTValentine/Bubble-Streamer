#include <iostream>

#include <QApplication>
#include <QSurfaceFormat>
#include <QDebug>
#include <QMessageBox>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <bubble.h>
#include <bubble_solver.h>

#include <openvdb/openvdb.h>

void debugFormatVersion() {
  QSurfaceFormat form = QSurfaceFormat::defaultFormat();
  QSurfaceFormat::OpenGLContextProfile prof = form.profile();

  const char *profile =
    prof == QSurfaceFormat::CoreProfile ? "Core" :
    prof == QSurfaceFormat::CompatibilityProfile ? "Compatibility" :
    "None";

  printf("Requested format:\n");
  printf("  Version: %d.%d\n", form.majorVersion(), form.minorVersion());
  printf("  Profile: %s\n", profile);
}

void HelloOpenVDB()
{
  // Initialize the OpenVDB library.  Thi s must be called at least
  // once per program and may safely be called multiple times.
  openvdb::initialize();

  //openvdb::tools::VolumeToMesh ma(0, 0);
  //cout << "Pool size: " << ma.polygonPoolListSize() << endl;
}

int main(int argc, char *argv[]) {

  QApplication a(argc, argv);

  HelloOpenVDB();

  // Set OpenGL 3.2 and, optionally, 4-sample multisampling
  QSurfaceFormat format;
  format.setVersion(3, 2);
  format.setOption(QSurfaceFormat::DeprecatedFunctions, false);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setSamples(4);  // Uncomment for nice antialiasing. Not always supported.

  /*** AUTOMATIC TESTING: DO NOT MODIFY ***/
  /*** Check whether automatic testing is enabled */
  /***/ if (qgetenv("CIS277_AUTOTESTING") != nullptr) {
    format.setSamples(0);
  }

  QSurfaceFormat::setDefaultFormat(format);
  debugFormatVersion();

  //Eigen::Matrix<float, 2, 3> m1;


  MainWindow w;
  w.show();

  //important test comment

  return a.exec();
}
