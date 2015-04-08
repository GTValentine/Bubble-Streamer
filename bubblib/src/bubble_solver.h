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
  BubbleSolver(int ni, int nj, int nk, double width_x,
               double scattering_freq, double scattering_coef,
               double breakup_freq, double scattering_impact,
               double expected_radius, double stddev_radius,
               BubbleAgent* agent = nullptr);
  BubbleSolver(double wx, double wy, double wz, double dx,
               BubbleAgent *agent = nullptr);
  ~BubbleSolver();

  const std::list<Bubble>& get_bubbles() const {
    return bubbles_;
  }

  double get_dx() const { return fluid_.get_dx(); }

  int get_ni() const { return fluid_.get_ni(); }

  int get_nj() const { return fluid_.get_nj(); }

  int get_nk() const { return fluid_.get_nk(); }

  double scattering_freq  () const { return scattering_freq_  ; }
  double scattering_coef  () const { return scattering_coef_  ; }
  double breakup_freq     () const { return breakup_freq_     ; }
  double scattering_impact() const { return scattering_impact_; }
  double expected_radius  () const { return expected_radius_  ; }
  double stddev_radius    () const { return stddev_radius_    ; }

  void scattering_freq  (double v) { scattering_freq_   = v; }
  void scattering_coef  (double v) { scattering_coef_   = v; }
  void breakup_freq     (double v) { breakup_freq_      = v; }
  void scattering_impact(double v) { scattering_impact_ = v; }
  void expected_radius  (double v) {
    expected_radius_ = v;
    radius_distribution = std::normal_distribution<double>(expected_radius_, stddev_radius_);
  }
  void stddev_radius    (double v) {
    stddev_radius_ = v;
    radius_distribution = std::normal_distribution<double>(expected_radius_, stddev_radius_);
  }

  void advance(double dt);
  void advance_cfl();

  void generate_n_bubbles(int n);

  void add_bubble(const glm::vec3& pos, double radius = -1.0);
  void add_bubble(const Vec3d& pos, double radius = -1.0);

 private:
  BubbleSolver();

  FluidSim fluid_;
  std::list<Bubble> bubbles_;
  std::default_random_engine random_generator;
  std::normal_distribution<double> radius_distribution;


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
