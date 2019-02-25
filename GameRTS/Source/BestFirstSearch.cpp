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
  m_nodeGrid = m_pTiledMap;
  return false;
}

void BestFirstSearch::Destroy()
{
   m_nodeGrid = nullptr;
}

WALKSTATE::E BestFirstSearch::Update()
{
  if (m_open.size() > 0)
  {
    m_use = m_open.front().position;
    m_close.push_back({ m_use, m_open.front().parent });

    m_open.pop_front();
    m_nodeGrid->setVisited(m_use.x, m_use.y, true);

    if (m_use == m_end)
    {
      m_currentState = WALKSTATE::REACHEDGOAL;
      return m_currentState;
    }
    if (m_use != m_start)
    {
      m_nodeGrid->setMark(m_use.x, m_use.y, PFMARK::N);
    }

    int32 x, y;
    x = m_use.x + 1;
    y = m_use.y;
    if (m_use.x < (m_pTiledMap->getMapSize().x - 1))
    {
      visitGridNode(x, y);
    }

    x = m_use.x + 1;
    y = m_use.y + 1;
    if (m_use.x < (m_pTiledMap->getMapSize().x - 1) &&
      m_use.y < (m_pTiledMap->getMapSize().y - 1))
    {
      visitGridNode(x, y);
    }

    x = m_use.x;
    y = m_use.y + 1;
    if (m_use.y < (m_pTiledMap->getMapSize().y - 1))
    {
      visitGridNode(x, y);
    }

    x = m_use.x - 1;
    y = m_use.y + 1;
    if (m_use.y < (m_pTiledMap->getMapSize().y - 1) && m_use.x > 0)
    {
      visitGridNode(x, y);
    }

    x = m_use.x - 1;
    y = m_use.y;
    if (m_use.x > 0)
    {
      visitGridNode(x, y);
    }

    x = m_use.x - 1;
    y = m_use.y - 1;
    if (m_use.x > 0 && m_use.y > 0)
    {
      visitGridNode(x, y);
    }

    x = m_use.x;
    y = m_use.y - 1;
    if (m_use.y > 0)
    {
      visitGridNode(x, y);
    }

    x = m_use.x + 1;
    y = m_use.y - 1;
    if (m_use.y > 0 && m_use.x < (m_pTiledMap->getMapSize().x - 1))
    {
      visitGridNode(x, y);
    }

    m_currentState = WALKSTATE::STILLLOOKING;
    return m_currentState;
  }

  m_currentState = WALKSTATE::UNABLETOREACHGOAL;
  return m_currentState;
}

void BestFirstSearch::Render()
{}

void BestFirstSearch::Reset()
{
  m_open.clear();

  m_use = Vector2I::ZERO;

  ClearClose();

  int x, y;
  getStartPosition(x, y);
  m_start = m_StartPos;
  m_nodeGrid->setVisited(m_start.x, m_start.y, true);

  getEndPosition(x, y);
  m_end = m_EndPos;

  m_open.push_back({ m_start,m_start });
  m_currentState = WALKSTATE::STILLLOOKING;
}

void 
BestFirstSearch::PriorityQueue(Vector2I& v)
{
  uint32 distance = v.manhattanDist(m_end);

  for (std::list<NodeList>::iterator it = m_open.begin(); it != m_open.end(); ++it)
  {
    if (it->position == v)
    {
      return;
    }
    uint32 distance2 = it->position.manhattanDist(m_end);
    if (distance < distance2)
    {
      m_open.insert(it, { v, m_use });
      return;
    }
  }
  m_open.push_back({v, m_use});
}

void BestFirstSearch::visitGridNode(int32 x, int32 y)
{
  if (m_nodeGrid->getVisited(x, y) ||
    m_nodeGrid->getType(x, y) == TERRAIN_TYPE::kObstacle) {
    return;
  }

  Vector2I v(x, y);
  PriorityQueue(v);
}
