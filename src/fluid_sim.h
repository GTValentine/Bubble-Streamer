#ifndef FLUID_SIM_H_
#define FLUID_SIM_H_

#include <list>
#include <cmath>
#include <algorithm>

#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>
#include <3dGrid/util.h>
#include <3dGrid/array3.h>
#include <3dGrid/array3_utils.h>

#include "bubble.h"

using std::list;
using std::vector;

using Eigen::SparseMatrix;
using Eigen::ConjugateGradient;
using Eigen::VectorXf;

class FluidSim
{
  public:
    FluidSim(int n);
    ~FluidSim();

    // i goes from -1 to ni - 'ghost pressure' on solid walls. same for j and k
    float get_pressure(int i, int j, int k) const {return pressure_[(i + 1)*(nj_ + 2)*(nk_ + 2) + (j + 1)*(nk_ + 2) + k + 1];}
    float get_u(int i, int j, int k) const {return velocity_u_(i, j, k);} //u(i - 0.5, j,       k)
    float get_v(int i, int j, int k) const {return velocity_v_(i, j, k);} //v(i,       j - 0.5, k)
    float get_w(int i, int j, int k) const {return velocity_w_(i, j, k);} //w(i,       j,       k - 0.5)

    void test();

    void print() const;
  private:
    FluidSim();//{}

    int ni_, nj_, nk_;
    float dx_;

    //Fluid velocity
    Array3f velocity_u_;
    Array3f velocity_v_;
    Array3f velocity_w_;

    Array3f velocity_tmp_u_;
    Array3f velocity_tmp_v_;
    Array3f velocity_tmp_w_;

    list<Bubble> bubbles_;

    //Solver data
    ConjugateGradient<SparseMatrix<float>> solver_;
    SparseMatrix<float> matrix_;
    VectorXf rhs_; //right-hand-side
    VectorXf pressure_; //actually, this is pressure times dx/dt

    float cfl();

    void advance(float dt);
    void advect(float dt);
    void add_force(float dt);
    void project();

    float& u(int i, int j, int k) {return velocity_u_(i, j, k);} //u(i - 0.5, j,       k)
    float& v(int i, int j, int k) {return velocity_v_(i, j, k);} //v(i,       j - 0.5, k)
    float& w(int i, int j, int k) {return velocity_w_(i, j, k);} //w(i,       j,       k - 0.5)

    float& tmp_u(int i, int j, int k) {return velocity_tmp_u_(i, j, k);} //tmp_u(i - 0.5, j,       k)
    float& tmp_v(int i, int j, int k) {return velocity_tmp_v_(i, j, k);} //tmp_v(i,       j - 0.5, k)
    float& tmp_w(int i, int j, int k) {return velocity_tmp_w_(i, j, k);} //tmp_w(i,       j,       k - 0.5)

    Vec3f get_velocity(const Vec3f& position);
    Vec3f trace_rk2(const Vec3f& position, float dt);

    void init_matrix();
    void compute_rhs();
    };

#endif // FLUID_SIM_H_

