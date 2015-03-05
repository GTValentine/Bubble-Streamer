#ifndef BUBBLE_H_
#define BUBBLE_H_

#include<list>

#include<glm/glm.hpp>

using glm::vec3;

class Bubble
{
  public:
    Bubble();
    ~Bubble();

  private:
    vec3 velocity_;
    vec3 position_;
    float radius_;
};

#endif // BUBBLE_H_

