#include "..\Include\Djistra.h"

#include <iostream>

/*
*/
Djistra::Djistra() :
  m_nodeGrid(nullptr)
{
  m_StartPos = Vector2I::ZERO;
  m_EndPos = Vector2I::ZERO;
}

/*
*/
Djistra::Djistra(RTSTiledMap* pMap) :
  GridWalker(pMap),
  m_nodeGrid(nullptr)
{}

/*
*/
Djistra::~Djistra()
{
  Destroy();
}

bool Djistra::Init()
{
  if (m_nodeGrid) {
    Destroy();
  }
  m_nodeGrid = m_pTiledMap;
  return false;

}

void Djistra::Destroy()
{
  //   if (nullptr != m_nodeGrid){
  //     delete m_nodeGrid;
  //   }
  m_nodeGrid = nullptr;
}

WALKSTATE::E Djistra::Update()
{
  if (m_open.size() > 0)
  {
    m_use = m_open.front().position;
    m_close.push_back({ m_use,m_open.front().parent });
    
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

void Djistra::Render()
{//TODO: Render
}

void Djistra::Reset()
{
  m_open.clear();

  m_use = Vector2I::ZERO;

  ClearClose();

  int x, y;
  getStartPosition(x, y);
  m_start = m_StartPos;
  m_nodeGrid->setVisited(m_start.x, m_start.y, true);

  //Obtenemos el punto final, obtenemos el nodo y lo marcamos como el nodo final
  getEndPosition(x, y);
  m_end = m_EndPos;

  //Agregamos el nodo inicial a la lista abierta
  m_open.push_back({ m_start,m_start });
  m_currentState = WALKSTATE::STILLLOOKING;
}

void Djistra::visitGridNode(int32 x, int32 y)
{
  if (m_nodeGrid->getVisited(x, y) ||
    m_nodeGrid->getType(x, y) == TERRAIN_TYPE::kObstacle) {
    return;
  }

  Vector2I v(x, y);

  for (std::list<NodeList>::iterator it = m_open.begin(); it != m_open.end(); ++it)
  {
    if (it->position == v)
    {
      return;
    }
  }

  m_open.push_back({v,m_use});
}
