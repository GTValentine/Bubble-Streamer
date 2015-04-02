#pragma once

#include <random>
#include <iostream>

using std::cout;
using std::endl;
using std::cin;

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "bubble_agent.h"
#include "fluid_sim.h"

double const WATER_DENSITY = 1000.0;
double const AIR_DENSITY = 1.25;

class BubbleSolver {
 public:
  BubbleSolver(int grid_resolution);
  BubbleSolver(int ni, int nj, int nk, double width_x,
               double scattering_freq, double scattering_coef,
               double breakup_freq, double scattering_impact,
               double expected_radius, double stddev_radius,
               BubbleAgent* agent);
  ~BubbleSolver();

  const std::list<Bubble>& get_bubbles() const {
    return bubbles_;
  }

  double get_dx() const {
    return fluid_.get_dx();
  }

  int get_ni() const {
    return fluid_.get_ni();
  }

  int get_nj() const {
    return fluid_.get_nj();
  }

  int get_nk() const {
    return fluid_.get_nk();
  }

  double& scattering_freq() {return scattering_freq_;}
  double& scattering_coef() {return scattering_coef_;}
  double& breakup_freq() {return breakup_freq_;}
  double& scattering_impact(){return scattering_impact_;}
  double& expected_radius(){return expected_radius_;}
  double& stddev_radius() {return stddev_radius_;}

  void advance(double dt);
  void advance_cfl();

  void generate_n_bubbles(int n);

  void add_bubble(const glm::vec3& pos, double radius);
  void add_bubble(const Vec3d& pos, double radius);

 private:
  BubbleSolver();

  FluidSim fluid_;
  std::list<Bubble> bubbles_;

  double scattering_freq_;
  double scattering_coef_;
  double breakup_freq_;
  double scattering_impact_;
  double expected_radius_;
  double stddev_radius_;

  BubbleAgent* agent_;

  void compute_density();

  void advance_bubbles(double dt);
  void compute_scattering_forces(double dt);

  double get_cos_scattering_angle() const;
  double get_scattering_probability(const Bubble& bubble) const;
  Vec3d get_scattering_force(const Bubble& bubble, double dt) const;

  static Vec3d get_random_point_cone_rim(const Vec3d& axis, double height, double radius);
};
