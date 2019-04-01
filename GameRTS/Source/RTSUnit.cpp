#include "RTSUnit.h"


namespace RTSGame{

RTSUnit::RTSUnit(const SPtr<RTSTexture>&  _texture,
                 const Vector<Animation>& _animation) :
        m_texture(_texture),
        m_animation(_animation),
        m_state(STATE::kIdle),
        m_direction(DIRECTIONS::kN),
        m_elapsedTime(0),
        m_frameCount(0) {}

RTSUnit::~RTSUnit()
{
}

/*
*/
void
RTSUnit::update(float deltaTime)
{
  m_elapsedTime += deltaTime;
}

/*
*/
void
RTSUnit::render(RTSTiledMap* tiledMap)
{

  float frameTime = m_animation[m_state].duration / m_animation[m_state].numFrames;
  AnimationFrame& currentFrame = m_animation[m_state].frames[m_direction][m_frameCount];
  m_texture->setSrcRect(currentFrame.x, currentFrame.y, currentFrame.w, currentFrame.h);

  int32 x, y;
  tiledMap->getMapToScreenCoords(m_position.x, m_position.y, x, y);
  m_texture->setPosition(x + (TILESIZE_X / 4),
                         y - currentFrame.h / 2);

  if(m_elapsedTime > frameTime)
  {
    m_frameCount = (++m_frameCount) % m_animation[m_state].numFrames;
    m_elapsedTime = 0.0f;
  }
  m_texture->draw();
}

void
RTSUnit::setPosition(float x, float y)
{
  m_position.x = x;
  m_position.y = y;
}

}
