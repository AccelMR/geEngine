#include "RTSWorld.h"
#include "RTSTiledMap.h"

#include "RTSUnitType.h"
#include "BestFirstSearch.h"
#include "DepthFirstSearch.h"
#include "BreadthFirstSearch.h"
#include "GridWalker.h"

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
  m_pTiledMap->init(m_pTarget, Vector2I(512, 512));

  //Create the path finding classes and push them to the walker list
  GridWalker* gw1 = new DepthFirstSearch(m_pTiledMap);
  gw1->setStartPosition(0, 0);
  gw1->setEndPosition(0, 0);

  GridWalker* gw2 = new BreadthFirstSearch(m_pTiledMap);
  gw2->setStartPosition(0, 0);
  gw2->setEndPosition(0, 0);

  m_walkersList.push_back(gw1);
  m_walkersList.push_back(gw2);

  //Init the walker objects
  for (SIZE_T it = 0; it < m_walkersList.size(); ++it) {
    m_walkersList[it]->Init();
  }

  //Set the first walker as the active walker
  setCurrentWalker(m_walkersList.size() > 0 ? 0 : -1);


  RTSGame::RTSUnitType unitTypes;
  unitTypes.loadAnimationData(m_pTarget, 1);

  return true;
}

void
RTSWorld::destroy() {
 //Destroy all the walkers
  while (m_walkersList.size() > 0) {
    ge_delete(m_walkersList.back());
    m_walkersList.pop_back();
  }

  //As the last step, destroy the full map
  if (nullptr != m_pTiledMap) {
    ge_delete(m_pTiledMap);
    m_pTiledMap = nullptr;
  }
}

void
RTSWorld::update(float deltaTime) {
  m_pTiledMap->update(deltaTime);
  if (m_activeWalker->GetState() == WALKSTATE::STILLLOOKING) 
  {
    m_activeWalker->Update();
  }
}

void
RTSWorld::render() {
  m_pTiledMap->render();
  m_activeWalker->Render();
}

void
RTSWorld::updateResolutionData() {
  if (nullptr != m_pTiledMap) {
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
{
  m_activeWalker->Reset();
}

void 
RTSWorld::SetStartPos(const int32 x, const int32 y)
{
  //Init the walker objects
  for (SIZE_T it = 0; it < m_walkersList.size(); ++it) {
    m_walkersList[it]->setStartPosition(x, y);
  }
  /*m_activeWalker->setStartPosition(x, y);*/
}

void 
RTSWorld::SetEndPos(const int32 x, const int32 y)
{
  //Init the walker objects
  for (SIZE_T it = 0; it < m_walkersList.size(); ++it) {
    m_walkersList[it]->setEndPosition(x, y);
  }
  /*m_activeWalker->setEndPosition(x, y);*/
}
