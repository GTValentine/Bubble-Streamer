#include "utils.h"

#include <iostream>
#include <QOpenGLFunctions_3_2_Core>


void printGLErrorLog()
{
    QOpenGLFunctions_3_2_Core f;
    f.initializeOpenGLFunctions();

    GLenum error = f.glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error " << error << ": ";
        const char *e =
            error == GL_INVALID_OPERATION             ? "GL_INVALID_OPERATION" :
            error == GL_INVALID_ENUM                  ? "GL_INVALID_ENUM" :
            error == GL_INVALID_VALUE                 ? "GL_INVALID_VALUE" :
            error == GL_INVALID_INDEX                 ? "GL_INVALID_INDEX" :
            error == GL_INVALID_OPERATION             ? "GL_INVALID_OPERATION" :
            QString::number(error).toUtf8().constData();
        std::cerr << e << std::endl;
        // Throwing here allows us to use the debugger to track down the error.
#ifndef __APPLE__
        // Don't do this on OS X.
        // http://lists.apple.com/archives/mac-opengl/2012/Jul/msg00038.html
        throw;
#endif
    }
}

void printLinkInfoLog(int prog)
{
    QOpenGLFunctions_3_2_Core f;
    f.initializeOpenGLFunctions();

    GLint linked;
    f.glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (linked == GL_TRUE) {
        return;
    }
    std::cerr << "GLSL LINK ERROR" << std::endl;

    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    f.glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        f.glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }
    // Throwing here allows us to use the debugger to track down the error.
    throw;
}

void printShaderInfoLog(int shader)
{
    QOpenGLFunctions_3_2_Core f;
    f.initializeOpenGLFunctions();

    GLint compiled;
    f.glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_TRUE) {
        return;
    }
    std::cerr << "GLSL COMPILE ERROR" << std::endl;

    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    f.glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        f.glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }
    // Throwing here allows us to use the debugger to track down the error.
    throw;
}
