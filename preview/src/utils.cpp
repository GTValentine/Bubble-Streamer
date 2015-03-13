#include "utils.h"

#include <iostream>

#include <QApplication>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_2_Core>


void printGLErrorLog() {
  QOpenGLFunctions &f = *QOpenGLContext::currentContext()->functions();

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

void printLinkInfoLog(int prog) {
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

void printShaderInfoLog(int shader) {
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

static inline const char *glGS(QOpenGLFunctions *f, GLenum e) {
  return reinterpret_cast<const char *>(f->glGetString(e));
}

void debugContextVersion() {
  QOpenGLContext *ctx = QOpenGLContext::currentContext();
  QOpenGLFunctions *f = ctx->functions();
  QSurfaceFormat ctxform = ctx->format();
  QSurfaceFormat::OpenGLContextProfile prof = ctxform.profile();

  const char *profile =
    prof == QSurfaceFormat::CoreProfile ? "Core" :
    prof == QSurfaceFormat::CompatibilityProfile ? "Compatibility" :
    "None";
  int ctxmajor = ctxform.majorVersion();
  int ctxminor = ctxform.minorVersion();
  bool valid = ctx->isValid();
  const char *vendor = glGS(f, GL_VENDOR);
  const char *renderer = glGS(f, GL_RENDERER);
  const char *version = glGS(f, GL_VERSION);
  const char *s_glsl = glGS(f, GL_SHADING_LANGUAGE_VERSION);

  printf("Context format version: %d.%d\n", ctxmajor, ctxminor);
  printf("Context valid: %s\n", valid ? "yes" : "NO");
  printf("Profile: %s\n", profile);
  printf("  Vendor:   %s\n", vendor);
  printf("  Renderer: %s\n", renderer);
  printf("  Version:  %s\n", version);
  printf("  GLSL:     %s\n", s_glsl);

  QString glsl = s_glsl;
  if (ctxmajor < 3 || glsl.startsWith("1.10") || glsl.startsWith("1.20")) {
    printf("ERROR: "
           "Unable to get an OpenGL 3.x context with GLSL 1.30 or newer.");
    QApplication::exit();
  }
}
