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
  matrix_(n*n*n, n*n*n), //TODO are you sure?
  rhs_(n*n*n),
  pressure_((n + 2)*(n + 2)*(n + 2))
{}

FluidSim::~FluidSim()
{}

//The main fluid simulation step
void FluidSim::advance(float dt)
{
  float t = 0, substep = 0;

  while(t < dt)
  {
    substep = cfl();
    if(t + substep > dt)
     substep = dt - t;

    advect(substep);
    add_force(substep);
    project(substep);

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
}

void FluidSim::project(float dt)
{



}

#endif // FLUID_SIM_CPP_
