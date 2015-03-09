#ifndef BUBBLESOLVER_H_
#define BUBBLESOLVER_H_

#include "fluid_sim.h"

double const WATER_DENSITY = 1000.0;
double const AIR_DENSITY = 1.25;

class BubbleSolver
{
  public:
    BubbleSolver(int grid_resolution);
    ~BubbleSolver();

    const list<Bubble>& get_bubbles() const {return bubbles_;}

    double get_dx() const {return fluid_.get_dx();}

    int get_ni() const {return fluid_.get_ni();}
    int get_nj() const {return fluid_.get_nj();}
    int get_nk() const {return fluid_.get_nk();}

  private:
    BubbleSolver(); //{}

    FluidSim fluid_;
    list<Bubble> bubbles_;

    void compute_density();
};

#endif // BUBBLESOLVER_H_
