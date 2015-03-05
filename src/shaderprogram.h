#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <glwidget277.h>
#include <la.h>

#include <QOpenGLFunctions_3_2_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>


class ShaderProgram
{
public:
    class Drawable
    {
    public:
        virtual GLenum drawMode() = 0;
        virtual int elemCount() = 0;
        virtual bool bindIdx() = 0;
        virtual bool bindPos() = 0;
        virtual bool bindNor() = 0;
        virtual bool bindCol() = 0;
    };

public:
    QOpenGLShaderProgram prog;

    int attrPos;
    int attrNor;
    int attrCol;

    int unifModel;
    int unifModelInvTr;
    int unifViewProj;

public:
    void create(const char *vertfile, const char *fragfile);
    void setModelMatrix(const la::mat4 &model);
    void draw(GLWidget277 &f, Drawable &d);
};


#endif // SHADERPROGRAM_H
