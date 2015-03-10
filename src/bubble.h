#ifndef BUBBLE_H_
#define BUBBLE_H_

#include <list>

#include <3dGrid/vec.h>
#include <pcgsolver/pcg_solver.h>

struct Bubble
{
    Vec3d velocity; //TODO: do you need this?
    Vec3d position;
    float radius;
};

#endif // BUBBLE_H_
