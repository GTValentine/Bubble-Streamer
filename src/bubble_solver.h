#ifndef BUBBLESOLVER_H_
#define BUBBLESOLVER_H_

#include <random>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

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

    void advance(double dt);

    static Vec3d get_random_point_cone_rim(const Vec3d& unit_axis, double height, double radius);

  private:
    BubbleSolver(); //{}

    FluidSim fluid_;
    list<Bubble> bubbles_;

    void compute_density();

    void advance_bubbles(double dt);
    void compute_scattering_forces(double dt);

};

#endif // BUBBLESOLVER_H_
