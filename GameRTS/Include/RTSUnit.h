#pragma once

#include "RTSUnitType.h"

namespace RTSGame{

namespace STATE{
enum E
{
  kAttack = 0,
  kDie,
  kIdle,
  kRun
};
}

class RTSUnit
{
public:
  RTSUnit(const SPtr<RTSTexture>& _texture,
          const Vector<Animation>& _animation);

  ~RTSUnit();

  void
  update(float deltaTime);

  void
  render();

  void
  setPosition(float x, float y);

private:
  sf::Vector2f m_position;

  float m_elapsedTime;

  uint32 m_frameCount;

  Vector<Animation> m_animation;

  const SPtr<RTSTexture> m_texture;

  STATE::E m_state;

  DIRECTIONS::E m_direction;

};
}

