#include "samplewire.h"
#include <la.h>

// **** THIS SECTION JUST PROVIDES SAMPLE DATA ****
static const int WIRE_IDX_COUNT = 6;
static const int WIRE_VERT_COUNT = 3;

static const GLuint wire_idx[WIRE_IDX_COUNT] = {
    0, 1,
    1, 2,
    2, 0,
};

static const la::vec3 wire_pos[WIRE_VERT_COUNT] = {
    {0, 0, 0}, {0, 0, 1}, {1, 0, 0},
};

static const la::vec3 wire_col[WIRE_VERT_COUNT] = {
    {1, 1, 0.5}, {0.5, 1, 1}, {1, 0.5, 1},
};
// ^^^^ THIS SECTION JUST PROVIDES SAMPLE DATA ^^^^


SampleWire::SampleWire()
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
}

void SampleWire::create()
{
    count = WIRE_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(wire_idx, WIRE_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(wire_pos, WIRE_VERT_COUNT * sizeof(la::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(wire_col, WIRE_VERT_COUNT * sizeof(la::vec3));
}

void SampleWire::destroy()
{
    bufIdx.destroy();
    bufPos.destroy();
    bufCol.destroy();
}

GLenum SampleWire::drawMode()
{
    return GL_LINES;
}

int SampleWire::elemCount()
{
    return count;
}

bool SampleWire::bindIdx()
{
    return bufIdx.bind();
}

bool SampleWire::bindPos()
{
    return bufPos.bind();
}

bool SampleWire::bindNor()
{
    return false;
}

bool SampleWire::bindCol()
{
    return bufCol.bind();
}
