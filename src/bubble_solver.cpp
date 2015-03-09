#include "bubble_solver.h"

BubbleSolver::BubbleSolver(int grid_resolution):
  fluid_(grid_resolution),
  bubbles_()
{
  compute_density();
}

BubbleSolver::~BubbleSolver()
{

}

void BubbleSolver::compute_density()
{
  fluid_.density().assign(WATER_DENSITY);

  int i = 0, j = 0, k = 0;
  double radius = 0;
  for(auto b = bubbles_.begin(); b != bubbles_.end(); ++b)
  {
    i = static_cast<int>(b->position[0]/get_dx());
    j = static_cast<int>(b->position[1]/get_dx());
    k = static_cast<int>(b->position[2]/get_dx());

    if(i >= 0 && i < get_ni() &&
       j >= 0 && j < get_nj() &&
       k >= 0 && k < get_nk())
    {
      radius = b->radius;
      fluid_.density(i, j, k) -= WATER_DENSITY*4.0/3.0*M_PI*radius/get_dx()*radius/get_dx()*radius/get_dx();
      if(fluid_.density(i, j, k) < 0) fluid_.density(i, j, k) = 0;
    }
  }
}
