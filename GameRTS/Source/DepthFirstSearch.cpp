#include "..\Include\DepthFirstSearch.h"

#include <iostream>

DepthFirstSearch::DepthFirstSearch()
{
}

DepthFirstSearch::DepthFirstSearch(RTSTiledMap * map) :
  GridWalker(map),
  m_nodeGrid(nullptr)
{}

DepthFirstSearch::~DepthFirstSearch()
{
}

bool DepthFirstSearch::Init()
{
  if (nullptr != m_nodeGrid) {
    Destroy();
  }
  m_nodeGrid = m_pTiledMap/*new RTSTiledMap(m_pTiledMap->getMapSize())*/;
  return false;
}

void DepthFirstSearch::Destroy()
{
//   if (nullptr != m_nodeGrid) {
//     delete m_nodeGrid;
//   }
   m_nodeGrid = nullptr;
}

WALKSTATE::E 
DepthFirstSearch::Update()
{
  if (m_open.size() > 0)
  {
    m_use = m_open.back().position;
    m_close.push_back({ m_use, m_open.back().parent });

    m_open.pop_back();
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

void DepthFirstSearch::Render()
{
}

void DepthFirstSearch::Reset()
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

void DepthFirstSearch::visitGridNode(int32 x, int32 y)
{
  Vector2I v(x, y);
  if (m_nodeGrid->getVisited(x, y) ||
      m_nodeGrid->getType(x, y) == TERRAIN_TYPE::kObstacle) 
  {
    return;
  }

  for (int i = 0; i < m_open.size(); ++i)
  {
    if (m_open[i].position == v)
    {
      return;
    }
  }
  m_open.push_back({ v, m_use });
}
