#include "trirenderer.h"


TriRenderer::TriRenderer()
    : bufIdx(QOpenGLBuffer::IndexBuffer)
    , bufPos(QOpenGLBuffer::VertexBuffer)
    , bufNor(QOpenGLBuffer::VertexBuffer)
    , bufCol(QOpenGLBuffer::VertexBuffer) {
}

TriRenderer::~TriRenderer() {
}

void TriRenderer::create() {
    bufIdx.create();
    bufPos.create();
    bufNor.create();
    bufCol.create();
}

void TriRenderer::destroy() {
    bufIdx.destroy();
    bufPos.destroy();
    bufNor.destroy();
    bufCol.destroy();
}

void TriRenderer::updateWith(const std::vector<vec3tri> &pos) {
    tricount = pos.size();
    std::vector<uint3> idx;
    std::vector<vec3tri> nor;
    std::vector<vec3tri> col;

    GLuint i = 0;
    glm::vec3 w(1.f, 1.f, 1.f);
    for (const vec3tri &p : pos) {
        glm::vec3 n = glm::cross(p.c - p.b, p.b - p.a);
        idx.push_back({i + 0, i + 1, i + 2});
        nor.push_back({n, n, n});
        col.push_back({w, w, w});
        i += 3;
    }

    int a1 = idx.size(), a2 = pos.size(), a3 = nor.size(), a4 = col.size();

    bufIdx.bind();
    bufIdx.allocate(idx.data(), idx.size() * sizeof(uint3));
    bufPos.bind();
    bufPos.allocate(pos.data(), pos.size() * sizeof(vec3tri));
    bufNor.bind();
    bufNor.allocate(nor.data(), nor.size() * sizeof(vec3tri));
    bufCol.bind();
    bufCol.allocate(col.data(), col.size() * sizeof(vec3tri));
}
