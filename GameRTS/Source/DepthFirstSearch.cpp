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
//   m_nodeGrid = nullptr;
}

WALKSTATE::E 
DepthFirstSearch::Update()
{
  if (m_open.size() > 0)
  {
    m_use = m_open.top();
    m_open.pop();
    m_nodeGrid->setVisited(m_use.x, m_use.y, true);

    if (m_use == m_start)
    {
      m_nodeGrid->setMark(m_use.x, m_use.y, PFMARK::START);
    }
    else
    {
      m_nodeGrid->setMark(m_use.x, m_use.y, PFMARK::N);
    }

    if (m_use == m_end)
    {
      m_nodeGrid->setMark(m_use.x, m_use.y, PFMARK::END);
      m_currentState = WALKSTATE::REACHEDGOAL;
      return m_currentState;
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
    if (m_use.x < (m_pTiledMap->getMapSize().x - 1) && m_use.y < (m_pTiledMap->getMapSize().y - 1))
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
  while (m_open.size() > 0)
  {
    m_open.pop();
  }

  m_use = Vector2I::ZERO;

  for (int32 i = 0; i < m_pTiledMap->getMapSize().x; i++)
  {
    for (int32 j = 0; j < m_pTiledMap->getMapSize().y; j++)
    {
      m_nodeGrid->setVisited(i, j, false);
      
      if (m_nodeGrid->getMark(i, j) == PFMARK::START ||
          m_nodeGrid->getMark(i, j) == PFMARK::END)
      {
        continue;
      }
      m_nodeGrid->setMark(i, j, PFMARK::NONE);
    }
  }

  int x, y;
  getStartPosition(x, y);
  m_start = m_StartPos;
  m_nodeGrid->setVisited(m_start.x, m_start.y, true);

  getEndPosition(x, y);
  m_end = m_EndPos;

  m_open.push(m_start);
  m_currentState = WALKSTATE::STILLLOOKING;
}

void DepthFirstSearch::visitGridNode(int32 x, int32 y)
{
  if (m_nodeGrid->getVisited(x, y) ||
      m_nodeGrid->getType(x, y) == TERRAIN_TYPE::kObstacle) 
  {
    return;
  }
  Vector2I v(x, y);
  m_open.push(v);

  std::cout << x << " " << y << std::endl;
}
