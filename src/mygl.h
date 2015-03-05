#ifndef MYGL_H
#define MYGL_H

#include <glwidget277.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/samplequad.h>
#include <scene/samplewire.h>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>


class MyGL
    : public GLWidget277
{
private:
    QOpenGLVertexArrayObject vao;

    SampleQuad geom_quad;
    SampleWire geom_wire;
    ShaderProgram prog_lambert;
    ShaderProgram prog_wire;

public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

protected:
    void keyPressEvent(QKeyEvent *e);
};


#endif // MYGL_H
