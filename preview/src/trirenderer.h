#pragma once

#include <vector>
#include <tuple>

#include <shaderprogram.h>

struct uint3 {
    GLuint a, b, c;
};

struct vec3tri {
    glm::vec3 a, b, c;
};


class TriRenderer
  : public ShaderProgram::Drawable
{
private:
  QOpenGLBuffer bufIdx;
  QOpenGLBuffer bufPos;
  QOpenGLBuffer bufNor;
  QOpenGLBuffer bufCol;
  int tricount;

public:
  TriRenderer();

  ~TriRenderer();

  void create();

  void destroy();

  void updateWith(const std::vector<vec3tri> &pos);

  // Drawable interface
public:
  GLenum drawMode() {
    return GL_LINE_LOOP;
    //return GL_TRIANGLES;  // Use this to render solid
  }

  int elemCount() { return tricount * 3; }

  bool bindIdx() { return bufIdx.bind(); }

  bool bindPos() { return bufPos.bind(); }

  bool bindNor() { return bufNor.bind(); }

  bool bindCol() { return bufCol.bind(); }
};
