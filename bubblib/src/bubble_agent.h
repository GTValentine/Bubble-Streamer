#pragma once

#include <random>
#include <cmath>

#include <3dGrid/vec.h>

class BubbleAgent
{
 public:
  BubbleAgent() : position_() {}
  BubbleAgent(const Vec3d& pos) : position_(pos){}
  virtual ~BubbleAgent();

  Vec3d& position() {return position_;}

  virtual Vec3d get_random_point() = 0;

 private:
  Vec3d position_;
};

class Disk : public BubbleAgent
{
 public:
  Disk();
  Disk(const Vec3d& pos, double rad) : BubbleAgent(pos), radius_(rad){}
  virtual ~Disk();

  double& radius() {return radius_;}

  virtual Vec3d get_random_point();

 private:
  double radius_;
};
