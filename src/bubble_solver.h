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
    void advance_cfl();

    static Vec3d get_random_point_cone_rim(const Vec3d& axis, double height, double radius);

    void seed_test_bubbles(int n);

  private:
    BubbleSolver(); //{}

    FluidSim fluid_;
    list<Bubble> bubbles_;

    double scattering_freq_;
    double scattering_coef_;
    double breakup_freq_;

    void compute_density();

    void advance_bubbles(double dt);
    void compute_scattering_forces(double dt);

    double get_cos_scattering_angle() const;
    double get_scattering_probability(const Bubble& bubble) const;
    Vec3d get_scattering_force(const Bubble& bubble, double dt) const;
};

#endif // BUBBLESOLVER_H_
