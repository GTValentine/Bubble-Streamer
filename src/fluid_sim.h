#ifndef FLUID_SIM_H_
#define FLUID_SIM_H_

#include <list>
#include <cmath>
#include <algorithm>

#include <3dGrid/util.h>
#include <3dGrid/array3.h>
#include <3dGrid/array3_utils.h>

#include "bubble.h"

using std::list;
using std::vector;

class FluidSim
{
  public:
    FluidSim(int n);
    ~FluidSim();

    double get_u(int i, int j, int k) const {return velocity_u_(i, j, k);} //u(i - 0.5, j,       k)
    double get_v(int i, int j, int k) const {return velocity_v_(i, j, k);} //v(i,       j - 0.5, k)
    double get_w(int i, int j, int k) const {return velocity_w_(i, j, k);} //w(i,       j,       k - 0.5)

    void test();

    void print() const;
  private:
    FluidSim();//{}

    int ni_, nj_, nk_;
    double dx_;

    //Fluid velocity
    Array3d velocity_u_;
    Array3d velocity_v_;
    Array3d velocity_w_;

    Array3d velocity_tmp_u_;
    Array3d velocity_tmp_v_;
    Array3d velocity_tmp_w_;

    list<Bubble> bubbles_;

    //Solver data
    PCGSolver<double> solver_;
    SparseMatrix<double> matrix_;
    vector<double> rhs_; //right-hand-side
    vector<double> pressure_; //actually, this is pressure times constant=dx/dt

    double cfl();

    void advance(double dt);
    void advect(double dt);
    void add_force(double dt);
    void project();

    double& u(int i, int j, int k) {return velocity_u_(i, j, k);} //u(i - 0.5, j,       k)
    double& v(int i, int j, int k) {return velocity_v_(i, j, k);} //v(i,       j - 0.5, k)
    double& w(int i, int j, int k) {return velocity_w_(i, j, k);} //w(i,       j,       k - 0.5)

    double& tmp_u(int i, int j, int k) {return velocity_tmp_u_(i, j, k);} //tmp_u(i - 0.5, j,       k)
    double& tmp_v(int i, int j, int k) {return velocity_tmp_v_(i, j, k);} //tmp_v(i,       j - 0.5, k)
    double& tmp_w(int i, int j, int k) {return velocity_tmp_w_(i, j, k);} //tmp_w(i,       j,       k - 0.5)

    Vec3d get_velocity(const Vec3d& position);
    Vec3d trace_rk2(const Vec3d& position, double dt);

    void init_matrix();
    void compute_rhs();
    };

#endif // FLUID_SIM_H_

