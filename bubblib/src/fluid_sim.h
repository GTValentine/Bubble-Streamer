#pragma once

#include <list>
#include <cmath>
#include <algorithm>

#include <3dGrid/util.h>
#include <3dGrid/array3.h>
#include <3dGrid/array3_utils.h>

#include "bubble.h"


class FluidSim {
 public:
  FluidSim(int n);
  FluidSim(int ni, int nj, int nk, double width_x);
  ~FluidSim();

  double get_u(int i, int j, int k) const {
    return velocity_u_(i, j, k); //u(i - 0.5, j,       k)
  }
  double get_v(int i, int j, int k) const {
    return velocity_v_(i, j, k); //v(i,       j - 0.5, k)
  }
  double get_w(int i, int j, int k) const {
    return velocity_w_(i, j, k); //w(i,       j,       k - 0.5)
  }

  double get_dx() const {
    return dx_;
  }

  int get_ni() const {
    return ni_;
  }
  int get_nj() const {
    return nj_;
  }
  int get_nk() const {
    return nk_;
  }

  void print() const;

  Array3d& density() {
    return density_;
  }
  double& density(int i, int j, int k) {
    return density_(i, j, k);
  }
  double get_density(int i, int j, int k) const {
    return density_(i, j, k);
  }

  double cfl() const;
  void advance(double dt);

  void set_zero_force();
  void set_zero_velocity();

  double& force_x(int i, int j, int k) {
    return extern_force_x_(i, j, k);
  }
  double& force_y(int i, int j, int k) {
    return extern_force_y_(i, j, k);
  }
  double& force_z(int i, int j, int k) {
    return extern_force_z_(i, j, k);
  }

  Vec3d get_velocity(const Vec3d& position) const;

 private:
  FluidSim();//{}

  int ni_, nj_, nk_;
  double dx_;

  Array3d density_;

  //Fluid velocity
  Array3d velocity_u_;
  Array3d velocity_v_;
  Array3d velocity_w_;

  Array3d velocity_tmp_u_;
  Array3d velocity_tmp_v_;
  Array3d velocity_tmp_w_;

  //Solver data
  PCGSolver<double> solver_;
  SparseMatrix<double> matrix_;
  std::vector<double> rhs_; //right-hand-side
  std::vector<double> pressure_; //actually, this is pressure times constant=dt/(dx*SCALING_CONST) (scaling is better this way)

  Array3d extern_force_x_;
  Array3d extern_force_y_;
  Array3d extern_force_z_;

  void advect(double dt);
  void add_force(double dt);
  void project();

  double& u(int i, int j, int k) {
    return velocity_u_(i, j, k); //u(i - 0.5, j,       k)
  }
  double& v(int i, int j, int k) {
    return velocity_v_(i, j, k); //v(i,       j - 0.5, k)
  }
  double& w(int i, int j, int k) {
    return velocity_w_(i, j, k); //w(i,       j,       k - 0.5)
  }

  double& tmp_u(int i, int j, int k) {
    return velocity_tmp_u_(i, j, k); //tmp_u(i - 0.5, j,       k)
  }
  double& tmp_v(int i, int j, int k) {
    return velocity_tmp_v_(i, j, k); //tmp_v(i,       j - 0.5, k)
  }
  double& tmp_w(int i, int j, int k) {
    return velocity_tmp_w_(i, j, k); //tmp_w(i,       j,       k - 0.5)
  }

  Vec3d trace_rk2(const Vec3d& position, double dt);

  void compute_matrix();
  void compute_rhs();
};
