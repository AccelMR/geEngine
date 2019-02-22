#include "..\Include\BestFirstSearch.h"

BestFirstSearch::BestFirstSearch()
{
}

BestFirstSearch::BestFirstSearch(RTSTiledMap * map):
  GridWalker(map),
  m_nodeGrid(nullptr)
{}

BestFirstSearch::~BestFirstSearch()
{}

bool BestFirstSearch::Init()
{
  if (m_nodeGrid) {
    Destroy();
  }
  m_nodeGrid = new RTSTiledMap(m_pTiledMap->getMapSize());
  return false;
}

void BestFirstSearch::Destroy()
{
  if (nullptr != m_nodeGrid) {
    delete m_nodeGrid;
  }
  m_nodeGrid = nullptr;
}

WALKSTATE::E BestFirstSearch::Update()
{
  return WALKSTATE::E();
}

void BestFirstSearch::Render()
{
}

void BestFirstSearch::Reset()
{
}

void BestFirstSearch::visitGridNode(int32 x, int32 y)
{
  if (m_nodeGrid->getVisited(x, y) ||
    m_nodeGrid->getType(x, y) == TERRAIN_TYPE::kObstacle) {
    return;
  }

  Vector2I v(x, y);
  m_open.push(v);
}
