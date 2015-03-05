#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent)
{
}

MyGL::~MyGL()
{
    makeCurrent();

    vao.destroy();
    geom_quad.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the point rendering size
    glPointSize(5);
    // Set the color which clears the screen between frames
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a VAO
    vao.create();

    // Create and set up the sample solid quad
    geom_quad.create();
    // Create and set up the sample wireframe triangle
    geom_wire.create();

    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the wireframe shader
    prog_wire.create(":/glsl/wire.vert.glsl", ":/glsl/wire.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    vao.bind();
}

void MyGL::resizeGL(int w, int h)
{
    // vvv TODO REPLACE THIS CODE
    la::mat4 proj = la::perspective(PI/4, w / (float) h, 0.1f, 100.f);
    la::mat4 view = la::lookAt(glm::vec3(-3, 4, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    la::mat4 viewproj = proj * view;
    // ^^^ TODO REPLACE THIS CODE

    // Upload the projection matrix
    QMatrix4x4 qviewproj = la::to_qmat(viewproj);

    prog_lambert.prog.bind();
    prog_lambert.prog.setUniformValue(prog_lambert.unifViewProj, qviewproj);

    prog_wire.prog.bind();
    prog_wire.prog.setUniformValue(prog_wire.unifViewProj, qviewproj);

    printGLErrorLog();
}

void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    la::mat4 model;

    prog_lambert.setModelMatrix(model);
    prog_lambert.draw(*this, geom_quad);

    prog_wire.setModelMatrix(model);
    prog_wire.draw(*this, geom_wire);

}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    }
}
