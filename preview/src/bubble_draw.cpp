#include "bubble_draw.h"


BubbleDraw::BubbleDraw()
  : solver(12)
  , box(&solver)
  , bub(&solver)
  , dirty(true)
{
  //for (int i = 0; i < 100; ++i) std::cout << BubbleSolver::get_random_point_cone_rim(Vec3d(0, 0, 1), 1, 0) << std::endl;

  //for (int i = 0; i < 100; ++i) {
  //    std::cout << solver.get_bubbles().back().position << std::endl;
  //    solver.advance(0.1);
  //}
}

void BubbleDraw::create()
{
  box.create();
  bub.create();
}

void BubbleDraw::destroy()
{
  box.destroy();
  bub.destroy();
}

void BubbleDraw::stepSim()
{
  int static frame = 0;
  solver.advance(0.1);
  if (frame < 450) {
    solver.seed_test_bubbles(10);
    ++frame;
  }
//    solver.advance_cfl();
  dirty = true;
}

void BubbleDraw::draw(ShaderProgram &p)
{
  if (dirty) {
    box.update();
    bub.update();
    dirty = false;
  }

  p.draw(box);
  p.draw(bub);
}


BubbleDraw::d_box::d_box(BubbleSolver *s)
  : solver(s)
  , bufIdx(QOpenGLBuffer::IndexBuffer)
  , bufPos(QOpenGLBuffer::VertexBuffer)
{
}

void BubbleDraw::d_box::create()
{
  bufIdx.create();
  bufIdx.bind();
  bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);

  bufPos.create();
  bufPos.bind();
  bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
}

void BubbleDraw::d_box::destroy()
{
  bufIdx.destroy();
  bufPos.destroy();
}

void BubbleDraw::d_box::update()
{
  double dx = solver->get_dx();
  glm::vec3 x2 = glm::vec3(solver->get_ni() * dx * 0.5, 0, 0);
  glm::vec3 y2 = glm::vec3(0, solver->get_nj() * dx * 0.5, 0);
  glm::vec3 z2 = glm::vec3(0, 0, solver->get_nk() * dx * 0.5);

  vector<GLuint> is;
  vector<glm::vec3> ps;
  count = 0;
  for (int _i = -1; _i <= 1; _i += 2) {
    float i = _i;
    for (int _j = -1; _j <= 1; _j += 2) {
      float j = _j;
      ps.push_back(i * x2 + j * y2 +     z2);
      ps.push_back(i * x2 + j * y2 -     z2);
      ps.push_back(j * x2 +     y2 + i * z2);
      ps.push_back(j * x2 -     y2 + i * z2);
      ps.push_back(    x2 + i * y2 + j * z2);
      ps.push_back(  - x2 + i * y2 + j * z2);
      for (int k = 0; k < 6; k++) {
        is.push_back(count);
        count++;
      }
    }
  }
  assert(count == 24);

  bufIdx.bind();
  bufIdx.allocate(&is[0], is.size() * sizeof(GLuint));
  bufPos.bind();
  bufPos.allocate(&ps[0], ps.size() * sizeof(glm::vec3));
}


BubbleDraw::d_bubbles::d_bubbles(BubbleSolver *s)
  : solver(s)
  , bufIdx(QOpenGLBuffer::IndexBuffer)
  , bufPos(QOpenGLBuffer::VertexBuffer)
  , bufCol(QOpenGLBuffer::VertexBuffer)
{
}

void BubbleDraw::d_bubbles::create()
{
  bufIdx.create();
  bufIdx.bind();
  bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);

  bufPos.create();
  bufPos.bind();
  bufPos.setUsagePattern(QOpenGLBuffer::StreamDraw);

  bufCol.create();
  bufCol.bind();
  bufCol.setUsagePattern(QOpenGLBuffer::StreamDraw);
}

void BubbleDraw::d_bubbles::destroy()
{
  bufIdx.destroy();
  bufPos.destroy();
  bufCol.destroy();
}

void BubbleDraw::d_bubbles::update()
{
  const list<Bubble> &bubs = solver->get_bubbles();
  count = bubs.size();

  double dx = solver->get_dx();
  Vec3d dim(solver->get_ni() * dx * 0.5,
            solver->get_nj() * dx * 0.5,
            solver->get_nk() * dx * 0.5);

  std::vector<GLuint> is;
  std::vector<glm::vec3> ps;
  std::vector<glm::vec3> cs;
  int i = 0;
  for (Bubble b : bubs) {
    ps.push_back(la::from_vec3d(b.position - dim));
    cs.push_back(glm::vec3(1, 1, 1));
    //std::cout << b.radius << std::endl;
    is.push_back(i);
    i++;
  }

  bufIdx.bind();
  bufIdx.allocate(&is[0], is.size() * sizeof(GLuint));
  bufPos.bind();
  bufPos.allocate(&ps[0], ps.size() * sizeof(glm::vec3));
  bufCol.bind();
  bufCol.allocate(&cs[0], cs.size() * sizeof(glm::vec3));
}
