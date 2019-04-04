#pragma once

#include <gePrerequisitesUtil.h>
#include <geVector2I.h>

#include <SFML/Graphics.hpp>

#include "RTSUnit.h"

using namespace geEngineSDK;


namespace RTSGame{
class RTSUnitType;
class RTSTiledMap;
class GridWalker;
class RTSTexture;
class RTSUnit;


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

  void
  addUnit(RTSGame::RTSUnit*);

  void
  createUnit(UNIT_TYPE::E unitType, int32 posX, int32 posY);
  
  void
  clearUnits();

  geEngineSDK::SPtr<RTSTexture> 
  getUnitTexture() const {
    return m_unitTexture;
  }

  void 
  fillSelectedVector(Vector2 topLftScrn, Vector2 botRightScrn);

  void
  setStartForUnits();

  void 
  resetSelected();

 private:
  RTSTiledMap* m_pTiledMap;

  SPtr<RTSTexture> m_unitTexture;
  SPtr<RTSTexture> m_selectedTex; //circle that says if unit was selected

  Vector<RTSGame::RTSUnitType*> m_lstUnitTypes;
  Vector<RTSGame::RTSUnit*> m_lstUnits;

  Vector<RTSUnit*> m_selectedUnits;

  Vector<Vector2I> m_path;
  sf::VertexArray m_drawPath;
  
  Vector<GridWalker*> m_walkersList;
/*  Vector<void*> m_walkersList;*/
  GridWalker* m_activeWalker;
  int8 m_activeWalkerIndex;

  sf::RenderTarget* m_pTarget;
};
}
