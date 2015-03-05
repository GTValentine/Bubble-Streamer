#include "samplequad.h"
#include <la.h>

// **** THIS SECTION JUST PROVIDES SAMPLE DATA ****
static const int QUAD_IDX_COUNT = 6;
static const int QUAD_VERT_COUNT = 4;

static const GLuint quad_idx[QUAD_IDX_COUNT] = {
    0, 1, 2,
    1, 2, 3,
};

static const la::vec3 quad_pos[QUAD_VERT_COUNT] = {
    {0, 0, 0}, {0, 1, 0}, {1, 0, 0}, {1, 1, 0},
};

static const la::vec3 quad_nor[QUAD_VERT_COUNT] = {
    {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1},
};

static const la::vec3 quad_col[QUAD_VERT_COUNT] = {
    {0, 0, 1}, {0, 1, 1}, {1, 0, 1}, {1, 1, 1},
};
// ^^^^ THIS SECTION JUST PROVIDES SAMPLE DATA ^^^^


SampleQuad::SampleQuad()
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufNor(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
}

void SampleQuad::create()
{
    count = QUAD_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(quad_idx, QUAD_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(quad_pos, QUAD_VERT_COUNT * sizeof(la::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(quad_nor, QUAD_VERT_COUNT * sizeof(la::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(quad_col, QUAD_VERT_COUNT * sizeof(la::vec3));
}

void SampleQuad::destroy()
{
    bufIdx.destroy();
    bufPos.destroy();
    bufNor.destroy();
    bufCol.destroy();
}

GLenum SampleQuad::drawMode()
{
    return GL_TRIANGLES;
}

int SampleQuad::elemCount()
{
    return count;
}

bool SampleQuad::bindIdx()
{
    return bufIdx.bind();
}

bool SampleQuad::bindPos()
{
    return bufPos.bind();
}

bool SampleQuad::bindNor()
{
    return bufNor.bind();
}

bool SampleQuad::bindCol()
{
    return bufCol.bind();
}
