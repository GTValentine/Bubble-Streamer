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

//! Simulator of dispersed bubble flow

//! Simulator of dispersed bubble flow based on a SIGGRAPH 2010 paper
//! "A Practical Simulation of Dispersed Bubble Flow" by
//! Doyub Kim, Oh-young Song, and Hyeong-Seok Ko. See the paper for more details.
class BubbleSolver {
 public:
  BubbleSolver(int ni /*! number of cells in X direction */,
               int nj /*! number of cells in Y direction */,
               int nk /*! number of cells in Z direction */,
               double dx /*! size of one grid cell */,
               double scattering_freq /*! scattering frequency. Characterizes the probabiliy of a given bubble to be scattered. Denoted as nu in the original paper. */,
               double scattering_coef /*! scattering coefficient. Characterizes the the direction of scattering. Denoted as k in the original paper. Should change from -1 to 1.*/,
               double breakup_freq /*! breakup frequency. Characterizes the expected fraction of bubble to break up at each frame. Denoted as gamma in the original paper. */,
               double scattering_impact /*! scattering impact. This parameter wasn't described in the original paper (in other words, in the original paper it equals one).
                                         *  Setting it greater than one increases the scattering force, making the flow less vertical and more curvy.*/,
               double expected_radius /*! whenever new bubble is generated, its radius is set to abs(N(mu, sigma)), where N is a normal distribution. This parameter is its mean.*/,
               double stddev_radius /*! whenever new bubble is generated, its radius is set to abs(N(mu, sigma)), where N is a normal distribution. This parameter is its sigma.*/,
               BubbleAgent* agent = nullptr /*! pointer to a source of bubbles*/);
  //! Create bubble solver with most parameters being predefined
  BubbleSolver(double wx /*! length of a container in X direction */,
               double wy /*! length of a container in Y direction */,
               double wz /*! length of a container in Z direction */,
               double dx /*! length of a grid cell */,
               BubbleAgent *agent = nullptr /*! pointer to a source of bubbles*/);
  ~BubbleSolver();

  //! Get a list of bubbles
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

  //! Advance the system by the time dt
  void advance(double dt);
  //! Advance the system by the maximum step allowed by CFL
  void advance_cfl();

  //! Generate n random bubbles on a surface of the agent
  void generate_n_bubbles(int n);

  //! Add a new bubble at a given position of a given radius. If radius isn't specified, it will be set randomly.
  void add_bubble(const glm::vec3& pos, double radius = -1.0);
  //! Add a new bubble at a given position of a given radius. If radius isn't specified, it will be set randomly.
  void add_bubble(const Vec3d& pos, double radius = -1.0);

 private:
  BubbleSolver();

  FluidSim fluid_;
  std::list<Bubble> bubbles_;
  std::default_random_engine random_generator_;
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

  double get_cos_scattering_angle();
  double get_scattering_probability(const Bubble& bubble) const;
  Vec3d get_scattering_force(const Bubble& bubble, double dt);

  Vec3d get_random_point_cone_rim(const Vec3d& axis, double height, double radius);
};
