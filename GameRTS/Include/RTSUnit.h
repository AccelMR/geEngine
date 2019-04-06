#pragma once

#include "RTSUnitType.h"
#include "RTSTiledMap.h"

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
          const Vector<Animation>& _animation,
          const SPtr<RTSTexture>& _selected);

  ~RTSUnit();

  void
  update(float deltaTime);

  void
  render(RTSTiledMap* tiledMap);

  void
  setPosition(float x, float y);

  void 
  setSelected(bool selected)
  {
    m_isSelected = selected;
  }

  sf::Vector2f
  getPosition() {
    return m_position;
  }

  void
  setPath(Vector<Vector2I> _path) {
    m_path = _path;
  }

private:
  sf::Vector2f m_position;

  float m_elapsedTime;

  uint32 m_frameCount;

  Vector<Animation> m_animation;

  Vector<Vector2I> m_path;

  const SPtr<RTSTexture> m_texture;
  const SPtr<RTSTexture> m_selectedTex;

  STATE::E m_state;

  DIRECTIONS::E m_direction;

  bool m_isSelected;

};
}

