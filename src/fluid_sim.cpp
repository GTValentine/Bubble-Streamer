#ifndef FLUID_SIM_CPP_
#define FLUID_SIM_CPP_

#include "fluid_sim.h"

FluidSim::FluidSim(int n):
  ni_(n),
  nj_(n),
  nk_(n),
  dx_(1.0f/n),
  velocity_u_(n + 1, n, n),
  velocity_v_(n, n + 1, n),
  velocity_w_(n, n, n + 1),
  velocity_tmp_u_(n + 1, n, n),
  velocity_tmp_v_(n, n + 1, n),
  velocity_tmp_w_(n, n, n + 1),
  bubbles_(),
  solver_(),
  matrix_(n*n*n, n*n*n),
  rhs_(n*n*n),
  pressure_(n*n*n)
{
  velocity_u_.set_zero();
  velocity_v_.set_zero();
  velocity_w_.set_zero();

  init_matrix();
  solver_.compute(matrix_);
}

FluidSim::~FluidSim()
{}

void FluidSim::init_matrix()
{
  int row = 0;
  int left = 0, right = 0;
  int top = 0, bottom = 0;
  int near = 0, far = 0;

  matrix_.reserve(Eigen::VectorXf::Constant(ni_*nj_*nk_, 7));
  for(int k = 0; k < nk_; ++k)
    for(int j = 0; j < nj_; ++j)
      for(int i = 0; i < ni_; ++i)
      {
        row = i*nj_*nk_ + j*nk_ + k;

        if(i > 0)
        {
          left = (i - 1)*nj_*nk_ + j*nk_ + k;
          matrix_.insert(row, left) = -1;
        }

        if(i < ni_ - 1)
        {
          right = (i + 1)*nj_*nk_ + j*nk_ + k;
          matrix_.insert(row, right) = -1;
        }

        if(j > 0)
        {
          bottom = i*nj_*nk_ + (j - 1)*nk_ + k;
          matrix_.insert(row, bottom) = -1;
        }

        if(j < nj_ - 1)
        {
          top = i*nj_*nk_ + (j + 1)*nk_ + k;
          matrix_.insert(row, top) = -1;
        }

        if(k > 0)
        {
          near = i*nj_*nk_ + j*nk_ + k - 1;
          matrix_.insert(row, near) = -1;
        }

        if(k < nk_ - 1)
        {
          far = i*nj_*nk_ + j*nk_ + k + 1;
          matrix_.insert(row, far) = -1;
        }

        matrix_.insert(row, row) = 6 - (i == 0) - (j == 0) - (k == 0) - (i == ni_ - 1) - (j == nj_ - 1) - (k == nk_ - 1);
      }

  matrix_.makeCompressed();

  //std::cout << matrix_ << std::endl;
}

//The main fluid simulation step
void FluidSim::advance(float dt)
{
  float t = 0, substep = 0;

  while(t < dt)
  {
    substep = cfl();
    if(t + substep > dt)
      substep = dt - t;

    //printf("step 0\n");
    //print();

    advect(substep);

    //printf("advected. step 1\n");
    //print();

    add_force(substep);

    //printf("foces added. step 2\n");
    //print();

    project();

    //printf("projected. done\n");
    //print();

    t += substep;
  }
}

float FluidSim::cfl()
{
  float maxvel = 0;

  for(unsigned int i = 0; i < velocity_u_.size(); ++i)
    maxvel = std::max(maxvel, fabsf(velocity_u_.a[i]));
  for(unsigned int i = 0; i < velocity_v_.size(); ++i)
    maxvel = std::max(maxvel, fabsf(velocity_v_.a[i]));
  for(unsigned int i = 0; i < velocity_w_.size(); ++i)
    maxvel = std::max(maxvel, fabsf(velocity_w_.a[i]));

  if(!maxvel) return 0.1f;

  return dx_ / maxvel;
}

void FluidSim::advect(float dt)
{
  Vec3f pos;

  //semi-Lagrangian advection on u-component of velocity
  for(int k = 0; k < nk_; ++k)
    for(int j = 0; j < nj_; ++j)
      for(int i = 0; i < ni_ + 1; ++i)
      {
        pos = Vec3f(i*dx_, (j + 0.5f)*dx_, (k + 0.5f)*dx_);
        pos = trace_rk2(pos, -dt);
        tmp_u(i, j, k) = get_velocity(pos)[0];
      }

  //semi-Lagrangian advection on v-component of velocity
  for(int k = 0; k < nk_; ++k)
    for(int j = 0; j < nj_ + 1; ++j)
      for(int i = 0; i < ni_; ++i)
      {
        pos = Vec3f((i + 0.5f)*dx_, j*dx_, (k + 0.5f)*dx_);
        pos = trace_rk2(pos, -dt);
        tmp_v(i, j, k) = get_velocity(pos)[1];
      }

  //semi-Lagrangian advection on w-component of velocity
  for(int k = 0; k < nk_ + 1; ++k)
    for(int j = 0; j < nj_; ++j)
      for(int i = 0; i < ni_; ++i)
      {
        pos = Vec3f((i + 0.5f)*dx_, (j + 0.5f)*dx_, k*dx_);
        pos = trace_rk2(pos, -dt);
        tmp_w(i, j, k) = get_velocity(pos)[2];
      }

  //move update velocities into u/v vectors
  velocity_u_ = velocity_tmp_u_;
  velocity_v_ = velocity_tmp_v_;
  velocity_w_ = velocity_tmp_w_;
}

Vec3f FluidSim::get_velocity(const Vec3f& position)
{
  float u_value = interpolate_value(position / dx_ - Vec3f(0, 0.5f, 0.5f), velocity_u_);
  float v_value = interpolate_value(position / dx_ - Vec3f(0.5f, 0, 0.5f), velocity_v_);
  float w_value = interpolate_value(position / dx_ - Vec3f(0.5f, 0.5f, 0), velocity_w_);

  return Vec3f(u_value, v_value, w_value);
}

Vec3f FluidSim::trace_rk2(const Vec3f& position, float dt)
{
  Vec3f velocity = get_velocity(position);
  velocity = get_velocity(position + 0.5f*dt*velocity);
  return position + dt*velocity;
}

void FluidSim::add_force(float dt)
{
  //gravity
  for(int k = 0; k < nk_; ++k)
    for(int j = 0; j < nj_ + 1; ++j)
      for(int i = 0; i < ni_; ++i)
        v(i,j,k) -= 9.81f * dt;
        //v(i,j,k) -= 10.0f * dt;
}

void FluidSim::project()
{
  compute_rhs();

  solver_.setTolerance(1.0e-10);
  solver_.setMaxIterations(1000);

  pressure_ = solver_.solve(rhs_);

  int row = 0;
  int left = 0;
  int bottom = 0;
  int near = 0;

  for(int k = 0; k < nk_; ++k)
    for(int j = 0; j < nj_; ++j)
      for(int i = 0; i < ni_ + 1; ++i)
      {
        row = i*nj_*nk_ + j*nk_ + k;
        left = (i - 1)*nj_*nk_ + j*nk_ + k;

        if(i == 0 || i == ni_)
          u(i, j, k) = 0;
        else
          u(i, j, k) -= (pressure_[row] - pressure_[left]);
      }

  for(int k = 0; k < nk_; ++k)
    for(int j = 0; j < nj_ + 1; ++j)
      for(int i = 0; i < ni_; ++i)
      {
        row = i*nj_*nk_ + j*nk_ + k;
        bottom = i*nj_*nk_ + (j - 1)*nk_ + k;

        if(j == 0 || j == nj_)
          v(i, j, k) = 0;
        else
        {
          v(i, j, k) -= (pressure_[row] - pressure_[bottom]);
          //printf("pressure_[row] - pressure_[bottom] = %f\n", pressure_[row] - pressure_[bottom]);
        }
      }

  for(int k = 0; k < nk_ + 1; ++k)
    for(int j = 0; j < nj_; ++j)
      for(int i = 0; i < ni_; ++i)
      {
        row = i*nj_*nk_ + j*nk_ + k;
        near = i*nj_*nk_ + j*nk_ + k - 1;

        if(k == 0 || k == nk_)
          w(i, j, k) = 0;
        else
          w(i, j, k) -= (pressure_[row] - pressure_[near]);
      }
  /*
  //TODO delete this, just testing
  for(int k = 0; k < nk_; ++k)
    for(int j = 0; j < nj_; ++j)
      for(int i = 0; i < ni_; ++i)
      {
        row = i*nj_*nk_ + j*nk_ + k;
        pressure_[row] *= dx_;
        pressure_[row] /= dt;
      }
  */
}

void FluidSim::compute_rhs()
{
  int row = 0;
  rhs_.setZero();

  //std::cout << "RHS:" << std::endl;

  for(int k = 0; k < nk_; ++k)
    for(int j = 0; j < nj_; ++j)
      for(int i = 0; i < ni_; ++i)
      {
        row = i*nj_*nk_ + j*nk_ + k;
        rhs_[row] -= get_u(i + 1, j, k)*(i != ni_ - 1) - get_u(i, j, k)*(i != 0);
        rhs_[row] -= get_v(i, j + 1, k)*(j != nj_ - 1) - get_v(i, j, k)*(j != 0);
        rhs_[row] -= get_w(i, j, k + 1)*(k != nk_ - 1) - get_w(i, j, k)*(k != 0);
        //std::cout << rhs_[row] << std::endl;
      }
  //std::cout << std::endl;
}

void FluidSim::test()
{
  advance(0.01f);

  // std::cout << fs.pressure_ << std::endl;
  for(int j = 0; j < nj_ + 1; ++j)
  {
    //std::cout << fs.pressure_[j*fs.nk_] << std::endl;
    std::cout << get_v(0, j, 0) << std::endl;
  }

}

void FluidSim::print() const
{
  printf("dx = %f\n", dx_);

  int row = 0;

  for(int k = 0; k < nk_; ++k)
    for(int j = 0; j < nj_; ++j)
      for(int i = 0; i < ni_; ++i)
      {
        row = i*nj_*nk_ + j*nk_ + k;
        printf("(%d, %d, %d): u=%f, v=%f, w=%f ", i, j, k, get_u(i, j, k), get_v(i, j, k), get_w(i, j, k));
        printf("p = %f\n", pressure_[row]);
      }

}


#endif // FLUID_SIM_CPP_

