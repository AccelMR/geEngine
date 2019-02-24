#pragma once

#include <gePrerequisitesUtil.h>
#include <geVector2I.h>

#include <SFML/Graphics.hpp>

using namespace geEngineSDK;

class RTSTiledMap;
class GridWalker;

class RTSWorld
{
 public:
  RTSWorld();
  ~RTSWorld();

 public:
  bool
  init(sf::RenderTarget* pTarget);

  void
  destroy();

  void
  update(float deltaTime);

  void
  render();

  RTSTiledMap*
  getTiledMap() {
    return m_pTiledMap;
  }

  void
  updateResolutionData();

  void
  setCurrentWalker(const int8 index);

  void
  ResetWalker();

  void
  SetStartPos(const int32 x, const int32 y);

  void
  SetEndPos(const int32 x, const int32 y);

 private:
  RTSTiledMap* m_pTiledMap;
  //List<RTSUnitType*> m_lstUnitTypes;
  //List<RTSUnit*> m_lstUnits;

  Vector<Vector2I> m_path;
  sf::VertexArray m_drawPath;
  
  Vector<GridWalker*> m_walkersList;
/*  Vector<void*> m_walkersList;*/
  GridWalker* m_activeWalker;
  int8 m_activeWalkerIndex;

  sf::RenderTarget* m_pTarget;
};
