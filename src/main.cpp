#include <iostream>

#include <mainwindow.h>
#include <ui_mainwindow.h>

#include <QApplication>
#include <QSurfaceFormat>
#include <QDebug>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "bubble.h"
#include "bubble_solver.h"

void debugFormatVersion()
{
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

int main(int argc, char *argv[])
{
    BubbleSolver bs(25);

    bs.seed_test_bubbles(1);

    //for(int i = 0; i < 100; ++i) std::cout << BubbleSolver::get_random_point_cone_rim(Vec3d(0, 0, 1), 1, 0) << std::endl;

    for(int i = 0; i < 100; ++i)
    {
      std::cout << bs.get_bubbles().back().position << std::endl;
      bs.advance(0.1);
    }

    QApplication a(argc, argv);

    // Set OpenGL 3.2 and, optionally, 4-sample multisampling
    QSurfaceFormat format;
    format.setVersion(3, 2);
    format.setOption(QSurfaceFormat::DeprecatedFunctions, false);
    format.setProfile(QSurfaceFormat::CoreProfile);
    //format.setSamples(4);  // Uncomment for nice antialiasing. Not always supported.

    /*** AUTOMATIC TESTING: DO NOT MODIFY ***/
    /*** Check whether automatic testing is enabled */
    /***/ if (qgetenv("CIS277_AUTOTESTING") != nullptr) format.setSamples(0);

    QSurfaceFormat::setDefaultFormat(format);
    debugFormatVersion();

    //Eigen::Matrix<float, 2, 3> m1;


    MainWindow w;
    w.show();

    //important test comment

    return a.exec();
}
