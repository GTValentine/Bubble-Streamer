#include "mygl.h"

#include <iostream>

#include <QApplication>
#include <QKeyEvent>
#include <QWheelEvent>


MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent)
    , camerasph(3, 0.3, -0.7) {
  // Update the simulation constantly as fast as possible
  // TODO: move to thread maybe
  connect(&simTimer, SIGNAL(timeout()), this, SLOT(stepSim()));
  simTimer.start(0);
}

MyGL::~MyGL() {
  makeCurrent();

  vao.destroy();
  soup.destroy();
}

void MyGL::initializeGL() {
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
  glPointSize(2);
  // Set the color which clears the screen between frames
  glClearColor(0.4f, 0.5f, 0.8f, 1.0f);

  printGLErrorLog();

  // Create a VAO
  vao.create();
  // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
  // using multiple VAOs, we can just bind one once.
  vao.bind();

  bubs.create();
  soup.create();

  // Create and set up the diffuse shader
  prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
  // Create and set up the wireframe shader
  prog_wire.create(":/glsl/wire.vert.glsl", ":/glsl/wire.frag.glsl");

  std::vector<vec3tri> tritest;
  vec3tri tritest1 = {
    glm::vec3(0.4f, 0.4f, 0.4f),
    glm::vec3(0.6f, 0.4f, 0.4f),
    glm::vec3(0.6f, 0.4f, 0.6f)
  };
  tritest.push_back(tritest1);
  soup.updateWith(tritest);
}

void MyGL::resizeGL(int, int) {
  updateCamera();
  printGLErrorLog();
}

void MyGL::paintGL() {
  // Clear the screen so that we only see newly drawn images
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  la::mat4 model;

  prog_lambert.setModelMatrix(model);
  prog_wire.setModelMatrix(model);
  bubs.draw(prog_wire);
  prog_wire.draw(soup);
}

void MyGL::updateCamera() {
  const qreal retinaScale = devicePixelRatio();
  glViewport(0, 0, width() * retinaScale, height() * retinaScale);

  int w = width();
  int h = height();
  glm::mat4 proj = glm::perspective(PI / 4, w / (float) h, 0.1f, 1000.f);
  glm::vec3 cam(
    cameraxyz.x + camerasph.x * cos(camerasph.y) * sin(camerasph.z),
    cameraxyz.y + camerasph.x * sin(camerasph.y),
    cameraxyz.z + camerasph.x * cos(camerasph.y) * cos(camerasph.z)
  );
  glm::mat4 view = glm::lookAt(cam, cameraxyz, glm::vec3(0, 1, 0));
  glm::mat4 viewproj = proj * view;
  setViewProj(viewproj);
}

void MyGL::setViewProj(const glm::mat4 &viewproj) {
  // Upload the projection matrix
  QMatrix4x4 qviewproj = la::to_qmat(viewproj);

  prog_wire.prog.bind();
  prog_wire.prog.setUniformValue(prog_wire.unifViewProj, qviewproj);

  prog_lambert.prog.bind();
  prog_lambert.prog.setUniformValue(prog_lambert.unifViewProj, qviewproj);
}

void MyGL::keyPressEvent(QKeyEvent *e) {
  // http://doc.qt.io/qt-5/qt.html#Key-enum
  switch (e->key()) {
  case Qt::Key_Escape:
    QApplication::quit();
    break;
  case Qt::Key_A:
    cameraxyz += glm::rotateY(glm::vec3(-0.1, 0, 0), camerasph.z);
    updateCamera();
    break;
  case Qt::Key_D:
    cameraxyz += glm::rotateY(glm::vec3(+0.1, 0, 0), camerasph.z);
    updateCamera();
    break;
  case Qt::Key_W:
    cameraxyz += glm::rotateY(glm::vec3(0, 0, -0.1), camerasph.z);
    updateCamera();
    break;
  case Qt::Key_S:
    cameraxyz += glm::rotateY(glm::vec3(0, 0, +0.1), camerasph.z);
    updateCamera();
    break;
  case Qt::Key_F:
    cameraxyz += glm::vec3(0, -0.1, 0);
    updateCamera();
    break;
  case Qt::Key_R:
    cameraxyz += glm::vec3(0, +0.1, 0);
    updateCamera();
    break;
  }
}

void MyGL::mousePressEvent(QMouseEvent *e) {
  mouselast = e->pos();
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
  if (e->buttons() & Qt::LeftButton) {
    auto pos = e->pos();
    QPoint diff = pos - mouselast;
    mouselast = e->pos();

    camerasph += glm::vec3(0, diff.y() * 0.005, -diff.x() * 0.005);
    updateCamera();
  }
}

void MyGL::wheelEvent(QWheelEvent *e) {
  auto delta = e->angleDelta();
  if (!delta.isNull()) {
    camerasph = glm::vec3(glm::max(1.f, camerasph.x + delta.y() * -0.005f), camerasph.y, camerasph.z);
    updateCamera();
  }
}

void MyGL::stepSim() {
  bubs.stepSim();
}
