#ifndef SAMPLEQUAD_H
#define SAMPLEQUAD_H

#include <shaderprogram.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>


class SampleQuad
    : public ShaderProgram::Drawable
{
private:
    int count;
    QOpenGLBuffer bufIdx;
    QOpenGLBuffer bufPos;
    QOpenGLBuffer bufNor;
    QOpenGLBuffer bufCol;

public:
    SampleQuad();

    void create();
    void destroy();

    virtual GLenum drawMode();
    virtual int elemCount();
    virtual bool bindIdx();
    virtual bool bindPos();
    virtual bool bindNor();
    virtual bool bindCol();
};


#endif // SAMPLEQUAD_H
