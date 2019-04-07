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
  update(float deltaTime, RTSTiledMap* tiledMap);

  void
  render(RTSTiledMap* tiledMap);

  void
  setPosition(float x, float y);

  void
  getTileCoord(const int32 mapX,
               const int32 mapY,
               int32 &scrX,
               int32 &scrY);

  void
  setMove(bool move) {
    m_gottaMove = move;
  }

  void 
  setSelected(bool selected)
  {
    m_isSelected = selected;
  }

  Vector2
  getPosition() {
    return m_tiledPos;
  }

  void
  setPath(Vector<Vector2I> _path) {
    m_path = _path;
    m_movingTo = m_path.back();
    m_path.pop_back();
  }

private:
  Vector2 m_tiledPos;

  float m_elapsedTime;

  float m_velocity;

  uint32 m_frameCount;

  Vector<Animation> m_animation;

  Vector<Vector2I> m_path;
  Vector2I m_movingTo;

  const SPtr<RTSTexture> m_texture;
  const SPtr<RTSTexture> m_selectedTex;

  STATE::E m_state;

  DIRECTIONS::E m_direction;

  bool m_isSelected;

  bool m_gottaMove;

};
}

