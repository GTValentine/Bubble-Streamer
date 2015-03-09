#ifndef BUBBLE_H_
#define BUBBLE_H_

#include <list>

#include <glm/glm.hpp>
#include <3dGrid/vec.h>
#include <pcgsolver/pcg_solver.h>

struct Bubble
{
    Vec3f velocity; //TODO: do you need this?
    Vec3f position;
    float radius;
};

#endif // BUBBLE_H_
