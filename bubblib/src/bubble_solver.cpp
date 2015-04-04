#include "bubble_solver.h"

BubbleSolver::BubbleSolver(int ni, int nj, int nk, double width_x,
    double scattering_freq, double scattering_coef, double breakup_freq, double scattering_impact,
    double expected_radius, double stddev_radius,
    BubbleAgent* agent):
  fluid_(ni, nj, nk, width_x),
  bubbles_(),
  scattering_freq_(scattering_freq),
  scattering_coef_(scattering_coef),
  breakup_freq_(breakup_freq),
  scattering_impact_(scattering_impact),
  expected_radius_(expected_radius),
  stddev_radius_(stddev_radius),
  agent_(agent){
  compute_density();
}

BubbleSolver::BubbleSolver(int grid_resolution):
  fluid_(grid_resolution),
  bubbles_(),
  scattering_freq_(10.0),
  scattering_coef_(0.9),
  breakup_freq_(0.001),
  scattering_impact_(100000),
  expected_radius_(0.001),
  stddev_radius_(0.0001),
  agent_(NULL){
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
      if (fluid_.density(i, j, k) < 0.42*WATER_DENSITY) {
        fluid_.density(i, j, k) = 0.42*WATER_DENSITY;
      }

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
  //const Vec3d container_dim(get_dx()*get_ni(), get_dx()*get_nj(), get_dx()*get_nk());

  for (auto b = bubbles_.begin(); b != bubbles_.end(); ++b) {
    b->position += fluid_.get_velocity(b->position) * dt;
    /*
    if (b->position[0] < 0 ||
        b->position[1] < 0 ||
        b->position[2] < 0 ||
        b->position[0] >= container_dim[0] ||
        b->position[1] >= container_dim[1] ||
        b->position[2] >= container_dim[2]) {
      b = bubbles_.erase(b);
    }
    */
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

void BubbleSolver::generate_n_bubbles(int n)
{
  static std::default_random_engine generator;
  std::normal_distribution<double> distribution(expected_radius_, stddev_radius_);

  Bubble b;

  //std::cout << agent_ << std::endl;

  if(agent_) {
    for (int i = 0; i < n; ++i) {
      b.position = agent_->get_random_point();
      //b.position[0] = get_ni()/2*get_dx() + get_dx()*distribution(generator);
      //b.position[1] = 0.5*get_dx();
      //b.position[2] = get_nk()/2*get_dx() + get_dx()*distribution(generator);
      b.radius = fabs(distribution(generator));
      bubbles_.push_back(b);
    }
  }
}

void BubbleSolver::add_bubble(const glm::vec3& pos, double radius)
{
  add_bubble(Vec3d(pos[0], pos[1], pos[2]), radius);
}

void BubbleSolver::add_bubble(const Vec3d& pos, double radius)
{
  Bubble b;
  b.position = pos;
  b.radius = radius;
  bubbles_.push_back(b);
}

Vec3d BubbleSolver::get_scattering_force(const Bubble& bubble, double dt) const {
  double cos_theta = get_cos_scattering_angle();
  Vec3d velocity = fluid_.get_velocity(bubble.position);

  double magnitude = sqrt(velocity[0] * velocity[0] + velocity[1] * velocity[1] + velocity[2] * velocity[2]);

  Vec3d a_velocity = get_random_point_cone_rim(velocity, magnitude * cos_theta, magnitude * sqrt(1.0 - cos_theta * cos_theta));

  return AIR_DENSITY * 4.0 * M_PI / 3.0 * bubble.radius * bubble.radius * bubble.radius / dt * (a_velocity - velocity) * scattering_impact_;
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
