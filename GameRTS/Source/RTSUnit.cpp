#include "RTSUnit.h"


namespace RTSGame{

  RTSUnit::RTSUnit(const SPtr<RTSTexture>& _texture,
                   const Vector<Animation>& _animation,
                   const SPtr<RTSTexture>& _selected) :
    m_texture(_texture),
    m_animation(_animation),
    m_state(STATE::kIdle),
    m_direction(DIRECTIONS::kN),
    m_elapsedTime(0),
    m_frameCount(0),
    m_isSelected(false),
    m_selectedTex(_selected),
    m_gottaMove(false),
    m_velocity(5)
  {
#ifdef MAP_IS_ISOMETRIC
    m_selectedTex->setScale(.028, .03 / 2);
    m_selectedTex->setRotation(Degree(30));
#else
    m_selectedTex->setScale(.03, .03);
#endif // MAP_IS_ISOMETRIC
    m_selectedTex->setOrigin(m_selectedTex->getWidth() / 2.f,
                             m_selectedTex->getHeight() / 2.f);
  }

RTSUnit::~RTSUnit()
{
}

/*
*/
void
RTSUnit::update(float deltaTime, RTSTiledMap* tiledMap)
{
  m_elapsedTime += deltaTime;

  if (m_gottaMove)
  {
    m_state = STATE::kRun;
//     int32 x, y;
//     tiledMap->getMapToScreenCoords(m_movingTo.x, m_movingTo.y, x, y);
    Vector2 v(m_movingTo.x, m_movingTo.y);

    float trashx, trashy;
    float Rx = std::modf(m_tiledPos.x, &trashx);
    float Ry = std::modf(m_tiledPos.y, &trashy);

    Vector2 tiled(trashx, trashy);
    if(Math::isNearlyEqual(v.x, m_tiledPos.x, .1f))
    {
      tiled.x = v.x;
    }
    if(Math::isNearlyEqual(v.y, m_tiledPos.y, .1f))
    {
      tiled.y = v.y;
    }

    if (tiled != v)
    {
      Vector2 dir = v - m_tiledPos;
      dir.normalize();
      m_tiledPos += dir * deltaTime * m_velocity;
    }
    else
    {
      if (m_path.empty())
      {
        m_gottaMove = false;
      }
      else
      {
        m_movingTo = m_path.back();
        m_path.pop_back();
      }
    }

  }
  else
  {
    m_state = STATE::kIdle;
  }
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
  float trashx, trashy;
  float Rx = std::modf(m_tiledPos.x, &trashx);
  float Ry = std::modf(m_tiledPos.y, &trashy);
  Rx *= TILESIZE_X;
  Ry *= TILESIZE_Y;

  tiledMap->getMapToScreenCoords(m_tiledPos.x, m_tiledPos.y, x, y);

#ifdef MAP_IS_ISOMETRIC
  m_texture->setPosition(x + Rx - (currentFrame.w / 2),
                         y + Ry - currentFrame.h);

#else
  m_texture->setPosition((x - currentFrame.w / 2) + Rx , (y - currentFrame.h) + Ry );
#endif  // MAP_IS_ISOMETRIC

  if(m_elapsedTime > frameTime)
  {
    m_frameCount = (++m_frameCount) % m_animation[m_state].numFrames;
    m_elapsedTime = 0.0f;
  }

  if (m_isSelected)
  {
    m_selectedTex->setPosition(x + Rx /*+ TILESIZE_X / 2*/,
                               y  + Ry/*+ TILESIZE_Y / 2*/);

    m_selectedTex->draw();

  }
  m_texture->draw();
}

void
RTSUnit::setPosition(float x, float y)
{
  Vector2 m_worldPos;
  //set to center always, then it can be moved in world
  m_worldPos.x = x * TILESIZE_X + TILESIZE_X / 2;
  m_worldPos.y = y * TILESIZE_Y + TILESIZE_Y / 2;

  //set now tile position, the residue is world position normalized
  m_tiledPos.x = m_worldPos.x / TILESIZE_X;
  m_tiledPos.y = m_worldPos.y / TILESIZE_Y;
}

void 
RTSUnit::getTileCoord(const int32 mapX, const int32 mapY, 
                      int32 &scrX, int32 &scrY)
{

}

}
