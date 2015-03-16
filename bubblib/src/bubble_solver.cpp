#include "bubble_solver.h"


BubbleSolver::BubbleSolver(int grid_resolution)
    : fluid_(grid_resolution)
    , bubbles_()
    , scattering_freq_(10.0)
    , scattering_coef_(0.9)
    , breakup_freq_(0.001) {
  compute_density();
}

BubbleSolver::~BubbleSolver() {
}

void BubbleSolver::compute_density() {
  fluid_.density().assign(WATER_DENSITY);

  int i = 0, j = 0, k = 0;
  double radius = 0;
  for (auto b = bubbles_.begin(); b != bubbles_.end(); ++b) {
    i = static_cast<int>(b->position[0] / get_dx());
    j = static_cast<int>(b->position[1] / get_dx());
    k = static_cast<int>(b->position[2] / get_dx());

    if (i >= 0 && i < get_ni() &&
        j >= 0 && j < get_nj() &&
        k >= 0 && k < get_nk()) {
      radius = b->radius;
      fluid_.density(i, j, k) -= WATER_DENSITY * 4.0 / 3.0 * M_PI * radius / get_dx() * radius / get_dx() * radius / get_dx();
      if (fluid_.density(i, j, k) < 0) {
        fluid_.density(i, j, k) = 0;
      }

      //printf("%f ", fluid_.density(i, j, k));
    }
  }
}

void BubbleSolver::advance(double dt) {
  double t = 0, substep = 0;

  while (t < dt) {
    compute_density();
    substep = fluid_.cfl();
    //printf("substep = %f\n", substep);
    if (t + substep > dt) {
      substep = dt - t;
    }

    compute_scattering_forces(substep);
    fluid_.advance(substep);
    advance_bubbles(substep);

    t += substep;
  }
}

void BubbleSolver::advance_cfl() {
  double substep = fluid_.cfl();
  if (substep <= 0) {
    substep = 0.1;
  }

  compute_scattering_forces(substep);
  fluid_.advance(substep);
  advance_bubbles(substep);
}

void BubbleSolver::compute_scattering_forces(double dt) {
  static std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0, 1.0);

  fluid_.set_zero_force();
  int i = 0, j = 0, k = 0;
  Vec3d force;

  for (auto b = bubbles_.begin(); b != bubbles_.end(); ++b)
    if (distribution(generator) < get_scattering_probability(*b)) {
      //printf("yep, i get scattered\n");
      i = static_cast<int>(b->position[0] / get_dx());
      j = static_cast<int>(b->position[1] / get_dx());
      k = static_cast<int>(b->position[2] / get_dx());
      if (i >= 0 && i < get_ni() &&
          j >= 0 && j < get_nj() &&
          k >= 0 && k < get_nk()) {
        force = get_scattering_force(*b, dt);

        fluid_.force_x(i + 0, j + 0, k + 0) += 0.5 * force[0];
        fluid_.force_x(i + 1, j + 0, k + 0) += 0.5 * force[0];

        fluid_.force_y(i + 0, j + 0, k + 0) += 0.5 * force[1];
        fluid_.force_y(i + 0, j + 1, k + 0) += 0.5 * force[1];

        fluid_.force_z(i + 0, j + 0, k + 0) += 0.5 * force[2];
        fluid_.force_z(i + 0, j + 0, k + 1) += 0.5 * force[2];
      }
    }
}

void BubbleSolver::advance_bubbles(double dt) {
  const Vec3d container_dim(get_dx()*get_ni(), get_dx()*get_nj(), get_dx()*get_nk());

  for (auto b = bubbles_.begin(); b != bubbles_.end(); ++b) {
    b->position += fluid_.get_velocity(b->position) * dt;

    //printf("bubble: %f %f %f\n", b->position[0], b->position[1], b->position[2]);
    //printf("container: %f %f %f\n", container_dim[0], container_dim[1], container_dim[2]);

    if (b->position[0] < 0 ||
        b->position[1] < 0 ||
        b->position[2] < 0 ||
        b->position[0] >= container_dim[0] ||
        b->position[1] >= container_dim[1] ||
        b->position[2] >= container_dim[2]) {
      b = bubbles_.erase(b);
    }
  }
}

Vec3d BubbleSolver::get_random_point_cone_rim(const Vec3d& axis, double height, double radius) { //TODO can you make it bettter?
  static std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0, M_PI * 2);
  double phi = distribution(generator);

  glm::dvec3 res(radius * cos(phi), radius * sin(phi), height);

  glm::dvec3 glm_unit_axis(axis[0], axis[1], axis[2]);
  glm_unit_axis = glm::normalize(glm_unit_axis);

  glm::dvec3 normal = glm::cross(glm::dvec3(0, 0, 1), glm_unit_axis);

  if (glm::dot(normal, normal) < 1e-5) {
    return Vec3d(res[0], res[1], res[2]);
  }

  res = glm::rotate(res, acos(glm::dot(glm_unit_axis, glm::dvec3(0, 0, 1))), normal);

  return Vec3d(res[0], res[1], res[2]);
}

void BubbleSolver::seed_test_bubbles(int n) {
  static std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.05, 0.95);

  for (int i = 0; i < n; ++i) {
    bubbles_.push_back(Bubble());
    bubbles_.back().position[0] = 0.5 * get_dx() * get_ni() + 2.5 * distribution(generator) * get_dx();
    bubbles_.back().position[1] = get_dx() * 0.5;
    bubbles_.back().position[2] = 0.5 * get_dx() * get_ni() + 2.5 * distribution(generator) * get_dx();
    bubbles_.back().radius = 0.005f;
  }
}

Vec3d BubbleSolver::get_scattering_force(const Bubble& bubble, double dt) const {
  double cos_theta = get_cos_scattering_angle();
  Vec3d velocity = fluid_.get_velocity(bubble.position);

  double magnitude = sqrt(velocity[0] * velocity[0] + velocity[1] * velocity[1] + velocity[2] * velocity[2]);

  Vec3d a_velocity = get_random_point_cone_rim(velocity, magnitude * cos_theta, magnitude * sqrt(1.0 - cos_theta * cos_theta));

  return AIR_DENSITY * 4.0 * M_PI / 3.0 * bubble.radius * bubble.radius * bubble.radius / dt * (a_velocity - velocity) * 100000.0; //TODO x100000? wasn't in the paper
}

double BubbleSolver::get_scattering_probability(const Bubble& bubble) const {
  int i = static_cast<int>(bubble.position[0] / get_dx());
  int j = static_cast<int>(bubble.position[1] / get_dx());
  int k = static_cast<int>(bubble.position[2] / get_dx());

  Vec3d velocity = fluid_.get_velocity(bubble.position);

  glm::dvec3 glm_velocity(velocity[0], velocity[1], velocity[2]);

  if (i >= 0 && i < get_ni() &&
      j >= 0 && j < get_nj() &&
      k >= 0 && k < get_nk()) {
    return scattering_freq_ * AIR_DENSITY * (1.0 - fluid_.get_density(i, j, k) / WATER_DENSITY) * glm::dot(glm_velocity, glm_velocity);
  }

  return 0.0;
}

double BubbleSolver::get_cos_scattering_angle() const {
  static std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0, 1.0);

  double sigma = distribution(generator);

  return (2 * sigma + scattering_coef_ - 1.0) / (2 * scattering_coef_ * sigma - scattering_coef_ + 1);
}
