#include "bubble_agent.h"

BubbleAgent::~BubbleAgent(){
}

Disk::Disk():
  BubbleAgent(),
  radius_(0.1){
}

Disk::~Disk(){
}

Vec3d Disk::get_random_point()
{
  static std::default_random_engine generator;
  std::uniform_real_distribution<double> radial(0.0, 1.0);
  std::uniform_real_distribution<double> phi(0.0, 2.0*M_PI);

  double r = radius()*sqrt(radial(generator));
  double angle = phi(generator);

  return position() + Vec3d(r*cos(angle), 0, r*sin(angle));
}
