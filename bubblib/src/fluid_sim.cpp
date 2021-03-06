#include "fluid_sim.h"


using std::list;
using std::vector;

const double SCALING_FACTOR = 1000.0;

FluidSim::FluidSim(int ni, int nj, int nk, double dx):
  ni_(ni),
  nj_(nj),
  nk_(nk),
  dx_(dx),
  density_(ni, nj, nk),
  velocity_u_(ni + 1, nj, nk),
  velocity_v_(ni, nj + 1, nk),
  velocity_w_(ni, nj, nk + 1),
  velocity_tmp_u_(ni + 1, nj, nk),
  velocity_tmp_v_(ni, nj + 1, nk),
  velocity_tmp_w_(ni, nj, nk + 1),
  solver_(),
  matrix_(ni * nj * nk - 1, 7), //7 non-zero elements per row
  rhs_(ni * nj * nk - 1),
  pressure_(ni * nj * nk),
  extern_force_x_(ni + 1, nj, nk),
  extern_force_y_(ni, nj + 1, nk),
  extern_force_z_(ni, nj, nk + 1) {
  set_zero_velocity();
  set_zero_force();
}

FluidSim::FluidSim(int n):
    ni_(n),
    nj_(n),
    nk_(n),
    dx_(1.0 / n),
    density_(n, n, n),
    velocity_u_(n + 1, n, n),
    velocity_v_(n, n + 1, n),
    velocity_w_(n, n, n + 1),
    velocity_tmp_u_(n + 1, n, n),
    velocity_tmp_v_(n, n + 1, n),
    velocity_tmp_w_(n, n, n + 1),
    solver_(),
    matrix_(n * n * n - 1, 7), //7 non-zero elements per row
    rhs_(n * n * n - 1),
    pressure_(n * n * n),
    extern_force_x_(n + 1, n, n),
    extern_force_y_(n, n + 1, n),
    extern_force_z_(n, n, n + 1) {
  set_zero_velocity();
  set_zero_force();
}

FluidSim::~FluidSim() {
}

void FluidSim::set_zero_velocity() {
  extern_force_x_.set_zero();
  extern_force_y_.set_zero();
  extern_force_z_.set_zero();
}

void FluidSim::set_zero_force() {
  extern_force_x_.set_zero();
  extern_force_y_.set_zero();
  extern_force_z_.set_zero();
}

void FluidSim::compute_matrix() {
  matrix_.zero();

  int row = 0;
  int left = 0, right = 0;
  int top = 0, bottom = 0;
  int near = 0, far = 0;
  int last = ni_ * nj_ * nk_ - 1;

  for (int k = 0; k < nk_; ++k)
    for (int j = 0; j < nj_; ++j)
      for (int i = 0; i < ni_; ++i)
        if ((i != ni_ - 1) || (j != nj_ - 1) || (k != nk_ - 1)) {
          row = i * nj_ * nk_ + j * nk_ + k;

          if (i > 0) {
            left = (i - 1) * nj_ * nk_ + j * nk_ + k;
            if (left != last) {
              matrix_.add_to_element(row, left, -2.0 * SCALING_FACTOR / (density_(i, j, k) + density_(i - 1, j, k)));
            }
            matrix_.add_to_element(row, row, 2.0 * SCALING_FACTOR / (density_(i, j, k) + density_(i - 1, j, k)));
          }

          if (i < ni_ - 1) {
            right = (i + 1) * nj_ * nk_ + j * nk_ + k;
            if (right != last) {
              matrix_.add_to_element(row, right, -2.0 * SCALING_FACTOR / (density_(i, j, k) + density_(i + 1, j, k)));
            }
            matrix_.add_to_element(row, row, 2.0 * SCALING_FACTOR / (density_(i, j, k) + density_(i + 1, j, k)));
          }

          if (j > 0) {
            bottom = i * nj_ * nk_ + (j - 1) * nk_ + k;
            if (bottom != last) {
              matrix_.add_to_element(row, bottom, -2.0 * SCALING_FACTOR / (density_(i, j, k) + density_(i, j - 1, k)));
            }
            matrix_.add_to_element(row, row, 2.0 * SCALING_FACTOR / (density_(i, j, k) + density_(i, j - 1, k)));
          }

          if (j < nj_ - 1) {
            top = i * nj_ * nk_ + (j + 1) * nk_ + k;
            if (top != last) {
              matrix_.add_to_element(row, top, -2.0 * SCALING_FACTOR / (density_(i, j, k) + density_(i, j + 1, k)));
            }
            matrix_.add_to_element(row, row, 2.0 * SCALING_FACTOR / (density_(i, j, k) + density_(i, j + 1, k)));
          }

          if (k > 0) {
            near = i * nj_ * nk_ + j * nk_ + k - 1;
            if (near != last) {
              matrix_.add_to_element(row, near, -2.0 * SCALING_FACTOR / (density_(i, j, k) + density_(i, j, k - 1)));
            }
            matrix_.add_to_element(row, row, 2.0 * SCALING_FACTOR / (density_(i, j, k) + density_(i, j, k - 1)));
          }

          if (k < nk_ - 1) {
            far = i * nj_ * nk_ + j * nk_ + k + 1;
            if (far != last) {
              matrix_.add_to_element(row, far, -2.0 * SCALING_FACTOR / (density_(i, j, k) + density_(i, j, k + 1)));
            }
            matrix_.add_to_element(row, row, 2.0 * SCALING_FACTOR / (density_(i, j, k) + density_(i, j, k + 1)));
          }
        }

  //std::cout << matrix_ << std::endl;
}


//The main fluid simulation step
void FluidSim::advance(double dt) {
  advect(dt);
  add_force(dt);
  project();
}

double FluidSim::cfl() const {
  double maxvel = 0;

  for (unsigned int i = 0; i < velocity_u_.size(); ++i) {
    maxvel = std::max(maxvel, fabs(velocity_u_.a[i]));
  }
  for (unsigned int i = 0; i < velocity_v_.size(); ++i) {
    maxvel = std::max(maxvel, fabs(velocity_v_.a[i]));
  }
  for (unsigned int i = 0; i < velocity_w_.size(); ++i) {
    maxvel = std::max(maxvel, fabs(velocity_w_.a[i]));
  }

  if (!maxvel) {
    return 0.1f;
  }

//  printf("maxvel = %f\n", maxvel);

  return dx_ / maxvel;
}

void FluidSim::advect(double dt) {
  Vec3d pos;

  //semi-Lagrangian advection on u-component of velocity
  for (int k = 0; k < nk_; ++k)
    for (int j = 0; j < nj_; ++j)
      for (int i = 0; i < ni_ + 1; ++i) {
        pos = Vec3d(i * dx_, (j + 0.5) * dx_, (k + 0.5) * dx_);
        pos = trace_rk2(pos, -dt);
        tmp_u(i, j, k) = get_velocity(pos)[0];
      }

  //semi-Lagrangian advection on v-component of velocity
  for (int k = 0; k < nk_; ++k)
    for (int j = 0; j < nj_ + 1; ++j)
      for (int i = 0; i < ni_; ++i) {
        pos = Vec3d((i + 0.5) * dx_, j * dx_, (k + 0.5) * dx_);
        pos = trace_rk2(pos, -dt);
        tmp_v(i, j, k) = get_velocity(pos)[1];
      }

  //semi-Lagrangian advection on w-component of velocity
  for (int k = 0; k < nk_ + 1; ++k)
    for (int j = 0; j < nj_; ++j)
      for (int i = 0; i < ni_; ++i) {
        pos = Vec3d((i + 0.5) * dx_, (j + 0.5) * dx_, k * dx_);
        pos = trace_rk2(pos, -dt);
        tmp_w(i, j, k) = get_velocity(pos)[2];
      }

  //move update velocities into u/v vectors
  velocity_u_ = velocity_tmp_u_;
  velocity_v_ = velocity_tmp_v_;
  velocity_w_ = velocity_tmp_w_;
}

Vec3d FluidSim::get_velocity(const Vec3d& position) const {
  double u_value = interpolate_value(position / dx_ - Vec3d(0, 0.5, 0.5), velocity_u_);
  double v_value = interpolate_value(position / dx_ - Vec3d(0.5, 0, 0.5), velocity_v_);
  double w_value = interpolate_value(position / dx_ - Vec3d(0.5, 0.5, 0), velocity_w_);

  return Vec3d(u_value, v_value, w_value);
}

Vec3d FluidSim::trace_rk2(const Vec3d& position, double dt) {
  Vec3d velocity = get_velocity(position);
  velocity = get_velocity(position + 0.5 * dt * velocity);
  return position + dt * velocity;
}

void FluidSim::add_force(double dt) {
  for (int k = 0; k < nk_; ++k)
    for (int j = 0; j < nj_; ++j)
      for (int i = 0; i < ni_ + 1; ++i) {
        u(i, j, k) += extern_force_x_(i, j, k);
      }

  for (int k = 0; k < nk_; ++k)
    for (int j = 0; j < nj_ + 1; ++j)
      for (int i = 0; i < ni_; ++i) {
        v(i, j, k) += (extern_force_y_(i, j, k) - 9.81 * dt);
      }

  for (int k = 0; k < nk_ + 1; ++k)
    for (int j = 0; j < nj_; ++j)
      for (int i = 0; i < ni_; ++i) {
        w(i, j, k) += extern_force_z_(i, j, k);
      }
}

void FluidSim::project() {
  compute_rhs();
  compute_matrix();

  double tolerance = 0;
  int iterations = 0;
  solver_.set_solver_parameters(1e-18, 10000);
  //printf("Solver started\n");
  bool success = solver_.solve(matrix_, rhs_, pressure_, tolerance, iterations);
  //printf("Solver took %d iterations and had residual %e\n", iterations, tolerance);
  if (!success) {
    printf("WARNING: Pressure solve failed!************************************************\n");
  }

  pressure_.back() = 0;

  int row = 0;
  int left = 0;
  int bottom = 0;
  int near = 0;

  for (int k = 0; k < nk_; ++k)
    for (int j = 0; j < nj_; ++j)
      for (int i = 0; i < ni_ + 1; ++i) {
        row = i * nj_ * nk_ + j * nk_ + k;
        left = (i - 1) * nj_ * nk_ + j * nk_ + k;

        if (i == 0 || i == ni_) {
          u(i, j, k) = 0;
        } else {
          u(i, j, k) -= (pressure_[row] - pressure_[left]) * SCALING_FACTOR * 2.0 / (density_(i, j, k) + density_(i - 1, j, k));
        }
      }

  for (int k = 0; k < nk_; ++k)
    for (int j = 0; j < nj_ + 1; ++j)
      for (int i = 0; i < ni_; ++i) {
        row = i * nj_ * nk_ + j * nk_ + k;
        bottom = i * nj_ * nk_ + (j - 1) * nk_ + k;

        if (j == 0 || j == nj_) {
          v(i, j, k) = 0;
        } else {
          v(i, j, k) -= (pressure_[row] - pressure_[bottom]) * SCALING_FACTOR * 2.0 / (density_(i, j, k) + density_(i, j - 1, k));
        }
      }

  for (int k = 0; k < nk_ + 1; ++k)
    for (int j = 0; j < nj_; ++j)
      for (int i = 0; i < ni_; ++i) {
        row = i * nj_ * nk_ + j * nk_ + k;
        near = i * nj_ * nk_ + j * nk_ + k - 1;

        if (k == 0 || k == nk_) {
          w(i, j, k) = 0;
        } else {
          w(i, j, k) -= (pressure_[row] - pressure_[near]) * SCALING_FACTOR * 2.0 / (density_(i, j, k) + density_(i, j, k - 1));
        }
      }

  /*
  //TODO delete this, just testing
  for(int k = 0; k < nk_; ++k)
    for(int j = 0; j < nj_; ++j)
      for(int i = 0; i < ni_; ++i)
      {
        row = i*nj_*nk_ + j*nk_ + k;
        pressure_[row] *= dx_;
        pressure_[row] /= 0.01;
      }
  */
}

void FluidSim::compute_rhs() {
  int row = 0;

  for (int k = 0; k < nk_; ++k)
    for (int j = 0; j < nj_; ++j)
      for (int i = 0; i < ni_; ++i)
        if ((i != ni_ - 1) || (j != nj_ - 1) || (k != nk_ - 1)) {
          row = i * nj_ * nk_ + j * nk_ + k;
          rhs_[row] = 0;
          rhs_[row] += - get_u(i + 1, j, k) * (i != ni_ - 1) + get_u(i, j, k) * (i != 0);
          rhs_[row] += - get_v(i, j + 1, k) * (j != nj_ - 1) + get_v(i, j, k) * (j != 0);
          rhs_[row] += - get_w(i, j, k + 1) * (k != nk_ - 1) + get_w(i, j, k) * (k != 0);
//          printf("(i, j, k) = (%d, %d, %d), (j != nj_ - 1) = %d, (j != 0) = %d, rhs_[row]  = %f\n", i, j, k, j != nj_ - 1, j != 0, rhs_[row]);
//          printf("rhs_[row]  = %f\n", rhs_[row]);
        }
}

void FluidSim::print() const {
  printf("dx = %f\n", dx_);

  int row = 0;

  for (int k = 0; k < nk_; ++k)
    for (int j = 0; j < nj_; ++j)
      for (int i = 0; i < ni_; ++i) {
        row = i * nj_ * nk_ + j * nk_ + k;
        printf("(%d, %d, %d): u=%f, v=%f, w=%f ", i, j, k, get_u(i, j, k), get_v(i, j, k), get_w(i, j, k));
        printf("p = %f\n", pressure_[row]);
      }

  printf("matrix\n");
  for (unsigned int j = 0; j < matrix_.n; ++j) {
    for (unsigned int i = 0; i < matrix_.n; ++i) {
      printf("%2f ", matrix_(i, j));
    }
    printf("\n");
  }
  printf("\n");
}
