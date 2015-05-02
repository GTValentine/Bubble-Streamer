#pragma once

#include <list>
#include <cmath>
#include <algorithm>

#include <3dGrid/util.h>
#include <3dGrid/array3.h>
#include <3dGrid/array3_utils.h>

//#include <openvdb/openvdb.h>
//#include <openvdb/tools/VolumeToMesh.h>

#include "bubble.h"

//! Grid-based fluid solver

//! Grid-based fluid solver based on Fluid3D library by Christopher Batty and
//! a book "Fluid Simulation for Computer Graphics" by Robert Bridson. It solves
//! a rectangular tank, full of fluid (no free surface) of varying density.
//! It's assumed that left bottom near corner of the tank is located at (0, 0, 0)
class FluidSim {
 public:
  FluidSim(int n /*! number of cells along each edge of a unit box container */);
  FluidSim(int ni /*! number of cells in X direction */,
           int nj /*! number of cells in Y direction */,
           int nk /*! number of cells in Z direction */,
           double dx /*! edge length of a grid cell */);
  ~FluidSim();

  //! get u-compunent of the fluid velocity at a point (i - 0.5, j, k)
  double get_u(int i, int j, int k) const {
    return velocity_u_(i, j, k); //u(i - 0.5, j,       k)
  }
  //! get v-compunent of the fluid velocity at a point (i, j - 0.5, k)
  double get_v(int i, int j, int k) const {
    return velocity_v_(i, j, k); //v(i,       j - 0.5, k)
  }
  //! get w-compunent of the fluid velocity at a point (i, j, k - 0.5)
  double get_w(int i, int j, int k) const {
    return velocity_w_(i, j, k); //w(i,       j,       k - 0.5)
  }

  //! get edge length of a grid cell
  double get_dx() const {
    return dx_;
  }

  //! get number of cells in X direction
  int get_ni() const {
    return ni_;
  }
  //! get number of cells in Y direction
  int get_nj() const {
    return nj_;
  }
  //! get number of cells in Z direction
  int get_nk() const {
    return nk_;
  }

  //! print the state of the tank (i.e. velocities, pressure, etc.)
  void print() const;

  //! grid of density
  Array3d& density() {
    return density_;
  }
  //! density at the center of a cell (i, j, k)
  double& density(int i, int j, int k) {
    return density_(i, j, k);
  }
  //! get density at the center of a cell (i, j, k)
  double get_density(int i, int j, int k) const {
    return density_(i, j, k);
  }

  //! maximum allowed time step according to CFL condition. If it's effectively infinite, 0.1 is returned
  double cfl() const;
  //! advance the fluid by the time step dt. If dt > CFL(), it will be broken into several smaller steps, each satisfying CFL
  void advance(double dt);

  //! set all external forces to zero
  void set_zero_force();
  //! set all velocities to zero
  void set_zero_velocity();

  //! get X-component of an external force, acting at a center of a cell (i, j, k)
  double& force_x(int i, int j, int k) {
    return extern_force_x_(i, j, k);
  }
  //! get Y-component of an external force, acting at a center of a cell (i, j, k)
  double& force_y(int i, int j, int k) {
    return extern_force_y_(i, j, k);
  }
  //! get Z-component of an external force, acting at a center of a cell (i, j, k)
  double& force_z(int i, int j, int k) {
    return extern_force_z_(i, j, k);
  }

  //! get velocity of a fluid at a given position by interpolating it from grid points
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
