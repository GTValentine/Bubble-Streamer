#pragma once

#include <glwidget277.h>
#include <utils.h>
#include <shaderprogram.h>
#include <bubble_draw.h>

#include <QTimer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>


class MyGL : public GLWidget277 {
  Q_OBJECT

 private:
  BubbleDraw bubs;

  QOpenGLVertexArrayObject vao;

  ShaderProgram prog_lambert;
  ShaderProgram prog_wire;

  la::vec3 cameraxyz;
  la::vec3 camerasph;
  QPoint mouselast;

  QTimer simTimer;

 public:
  explicit MyGL(QWidget *parent = 0);
  ~MyGL();

  void initializeGL();
  void resizeGL(int, int);
  void paintGL();

 protected:
  void updateCamera();
  void setViewProj(const glm::mat4 &viewproj);

  void keyPressEvent(QKeyEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void wheelEvent(QWheelEvent *e);

 private slots:
  void stepSim();
};
