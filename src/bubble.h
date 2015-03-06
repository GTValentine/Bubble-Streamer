#ifndef BUBBLE_H_
#define BUBBLE_H_

#include <list>

#include <glm/glm.hpp>
#include <3dGrid/vec.h>
#include <pcgsolver/pcg_solver.h>

class Bubble
{
  public:
    Bubble();
    ~Bubble();

  private:
    Vec3f velocity_;
    Vec3f position_;
    float radius_;
};

#endif // BUBBLE_H_

