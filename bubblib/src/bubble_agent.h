#pragma once

#include <random>
#include <cmath>

#include <3dGrid/vec.h>

//! Source of bubbles
class BubbleAgent
{
 public:
  BubbleAgent() : position_() {}
  BubbleAgent(const Vec3d& pos) : position_(pos){}
  virtual ~BubbleAgent();

  Vec3d& position() {return position_;}

  //! Returns random point on the surface of the agent, which is used as a starting position for a newly created bubble.
  virtual Vec3d get_random_point() = 0;

 private:
  Vec3d position_;
};

//! Source of bubbles having a shape of a disk, lying in XZ plane
class Disk : public BubbleAgent
{
 public:
  Disk();
  Disk(const Vec3d& pos /*! center of a disk */,
       double rad /*! radius of a disk */) : BubbleAgent(pos), radius_(rad){}
  virtual ~Disk();

  double& radius() {return radius_;}

  virtual Vec3d get_random_point();

 private:
  double radius_;
};
