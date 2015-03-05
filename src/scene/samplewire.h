#ifndef SAMPLEWIRE_H
#define SAMPLEWIRE_H

#include <shaderprogram.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>


class SampleWire
    : public ShaderProgram::Drawable
{
private:
    int count;
    QOpenGLBuffer bufIdx;
    QOpenGLBuffer bufPos;
    QOpenGLBuffer bufCol;

public:
    SampleWire();

    void create();
    void destroy();

    virtual GLenum drawMode();
    virtual int elemCount();
    virtual bool bindIdx();
    virtual bool bindPos();
    virtual bool bindNor();
    virtual bool bindCol();
};

#endif // SAMPLEWIRE_H
