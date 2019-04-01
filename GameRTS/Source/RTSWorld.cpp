#include "RTSWorld.h"
#include "RTSTiledMap.h"

#include "RTSUnitType.h"
#include "RTSTexture.h"
#include "BestFirstSearch.h"
#include "DepthFirstSearch.h"
#include "BreadthFirstSearch.h"
#include "Dijkstra.h"
#include "AStar.h"
#include "GridWalker.h"

namespace RTSGame{
RTSWorld::RTSWorld() {
  m_pTiledMap = nullptr;
  m_activeWalkerIndex = -1;	//-1 = Invalid index
}

RTSWorld::~RTSWorld() {
  destroy();
}

bool
RTSWorld::init(sf::RenderTarget* pTarget) {
  GE_ASSERT(nullptr == m_pTiledMap && "World was already initialized");
  destroy();

  m_pTarget = pTarget;

  //Initialize the map (right now it's an empty map)
  m_pTiledMap = ge_new<RTSTiledMap>();
  GE_ASSERT(m_pTiledMap);
  m_pTiledMap->init(m_pTarget, Vector2I(256, 256));

  //Create the path finding classes and push them to the walker list
  m_walkersList.push_back(ge_new<DepthFirstSearch>(m_pTiledMap));
  m_walkersList.push_back(ge_new<BreadthFirstSearch>(m_pTiledMap));
  m_walkersList.push_back(ge_new<BestFirstSearch>(m_pTiledMap));
  m_walkersList.push_back(ge_new<Dijkstra>(m_pTiledMap, m_pTarget));
  m_walkersList.push_back(ge_new<AStar>(m_pTiledMap));

  //Init the walker objects
  for(auto & it : m_walkersList)
  {
    it->Init();
    it->setStartPosition(0, 0);
    it->setEndPosition(0, 0);
  }

  //Set the first walker as the active walker
  setCurrentWalker(!m_walkersList.empty() ? 0 : -1);

  m_unitTexture = std::make_shared<RTSTexture>() ;
  m_unitTexture->loadFromFile(m_pTarget, "RTS/assets/game_objects/units/units.png");

  m_lstUnitTypes.push_back(ge_new<RTSGame::RTSUnitType>());
  m_lstUnitTypes.push_back(ge_new<RTSGame::RTSUnitType>());
  m_lstUnitTypes.push_back(ge_new<RTSGame::RTSUnitType>());

  for(uint16 i = 0; i < m_lstUnitTypes.size(); ++i)
  {
    m_lstUnitTypes[i]->loadAnimationData(m_pTarget, i + 1);
  }

  m_drawPath = sf::VertexArray(sf::LineStrip);

  return true;
}

void
RTSWorld::destroy() {
  //Destroy all the walkers
  while(!m_walkersList.empty())
  {
    ge_delete(m_walkersList.back());
    m_walkersList.pop_back();
  }

  //As the last step, destroy the full map
  if(nullptr != m_pTiledMap)
  {
    ge_delete(m_pTiledMap);
    m_pTiledMap = nullptr;
  }

  for(auto & it : m_lstUnits)
  {
    ge_delete(it);
  }
  for(auto & it : m_lstUnitTypes)
  {
    ge_delete(it);
  }

  m_lstUnitTypes.clear();
  m_lstUnits.clear();
  m_path.clear();
}

void
RTSWorld::update(float deltaTime) {
  m_pTiledMap->update(deltaTime);

  if(m_activeWalker->GetState() == WALKSTATE::STILLLOOKING)
  {
    m_activeWalker->Update();
  }

  else if(m_activeWalker->GetState() == WALKSTATE::REACHEDGOAL)
  {
    if(m_path.empty())
    {
      m_path = m_activeWalker->BackTracing();
      m_drawPath.resize(m_path.size());
    }

    int32  x, y;
    for(int32 i = 0; i < m_path.size(); ++i)
    {
      m_pTiledMap->getMapToScreenCoords(m_path[i].x, m_path[i].y, x, y);
      sf::Vertex v(sf::Vector2f(x + (TILESIZE_X >> 1), y + (TILESIZE_Y >> 1)), sf::Color::White);
      m_drawPath[i] = v;
    }
  }

  for(auto & it : m_lstUnits)
  {
    it->update(deltaTime);
  }

}

void
RTSWorld::render() {
  m_pTiledMap->render();
  m_activeWalker->Render();
  if(!m_path.empty())
  {
    m_pTarget->draw(m_drawPath);
  }

  for(auto & it : m_lstUnits)
  {
    it->render(m_pTiledMap);
  }
}

void
RTSWorld::updateResolutionData() {
  if(nullptr != m_pTiledMap)
  {
    Vector2I appResolution = g_gameOptions().s_Resolution;

    m_pTiledMap->setStart(0, 0);
    m_pTiledMap->setEnd(appResolution.x, appResolution.y - 175);

    //This ensures a clamp if necessary
    m_pTiledMap->moveCamera(0, 0);
  }
}

void
RTSWorld::setCurrentWalker(const int8 index) {
  //Revisamos que el walker exista (en modo de debug)
  GE_ASSERT(m_walkersList.size() > static_cast<SIZE_T>(index));

  m_activeWalker = m_walkersList[index];
  m_activeWalkerIndex = index;
}

void RTSWorld::ResetWalker()
{//TODO: Mejorar esto
  for(auto & it : m_walkersList)
  {
    it->Reset();
  }
  m_path.clear();
}

void
RTSWorld::SetStartPos(const int32 x, const int32 y)
{
  //Init the walker objects
  for(auto & it : m_walkersList)
  {
    it->setStartPosition(x, y);
  }
  /*m_activeWalker->setStartPosition(x, y);*/
}

void
RTSWorld::SetEndPos(const int32 x, const int32 y)
{
  //Init the walker objects
  for(auto & it : m_walkersList)
  {
    it->setEndPosition(x, y);
  }
  /*m_activeWalker->setEndPosition(x, y);*/
}

void
RTSWorld::addUnit(RTSGame::RTSUnit* unit)
{
  m_lstUnits.push_back(unit);
}

void
RTSWorld::createUnit(UNIT_TYPE::E unitType, int32 posX, int32 posY)
{
  auto* unit = ge_new<RTSUnit>(m_unitTexture, 
                               m_lstUnitTypes[unitType]->getAnimation());
  unit->setPosition(posX, posY);
  m_lstUnits.push_back(unit);
}

void
RTSWorld::clearUnits()
{
  for (auto& it : m_lstUnits)
  {
    ge_delete(it);
  }
  m_lstUnits.clear();
}

}