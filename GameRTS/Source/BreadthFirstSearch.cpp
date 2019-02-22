#include "..\Include\BreadthFirstSearch.h"

/*
*/
BreadthFirstSearch::BreadthFirstSearch():
  m_nodeGrid(nullptr)
{
  m_StartPos = Vector2I::ZERO;
  m_EndPos = Vector2I::ZERO;
}

/*
*/
BreadthFirstSearch::BreadthFirstSearch(RTSTiledMap* pMap) : 
  GridWalker(pMap),
  m_nodeGrid(nullptr)
{}

/*
*/
BreadthFirstSearch::~BreadthFirstSearch()
{
  Destroy();
}


bool BreadthFirstSearch::Init()
{
  if (m_nodeGrid) { 
    Destroy();
  }
  m_nodeGrid =  m_pTiledMap/*new RTSTiledMap(m_pTiledMap->getMapSize())*/;
  return false;

}

void BreadthFirstSearch::Destroy()
{
//   if (nullptr != m_nodeGrid){
//     delete m_nodeGrid;
//   }
//   m_nodeGrid = nullptr;
}

WALKSTATE::E BreadthFirstSearch::Update()
{
  if (m_currentState != WALKSTATE::REACHEDGOAL||
      m_currentState == WALKSTATE::UNABLETOREACHGOAL)
  {
    if (m_open.size() > 0)
    {
      m_use = m_open.front();
      m_open.pop_front();
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
}

void BreadthFirstSearch::Render()
{//TODO: Render
}

void BreadthFirstSearch::Reset()
{
  while (m_open.size() > 0)
  {
    m_open.pop_front();
  }

  m_use = Vector2I::ZERO;

  for (int32 i = 0; i < m_pTiledMap->getMapSize().x; i++)
  {
    for (int j = 0; j < m_pTiledMap->getMapSize().y; j++)
    {
      if (m_nodeGrid->getMark(i, j) == PFMARK::START ||
          m_nodeGrid->getMark(i, j) == PFMARK::END)
      {
        continue;
      }

      m_nodeGrid->setMark(i, j, PFMARK::NONE);
      m_nodeGrid->setVisited(i, j, false);
    }
  }

  int x, y;
  getStartPosition(x, y);
  m_start = m_StartPos;
  m_nodeGrid->setVisited(m_start.x, m_start.y, true);

  //Obtenemos el punto final, obtenemos el nodo y lo marcamos como el nodo final
  getEndPosition(x, y);
  m_end = m_EndPos;

  //Agregamos el nodo inicial a la lista abierta
  m_open.push_back(m_start);
  m_currentState = WALKSTATE::STILLLOOKING;
}

void BreadthFirstSearch::visitGridNode(int32 x, int32 y)
{
  if (m_nodeGrid->getVisited(x, y) ||
      m_nodeGrid->getType(x,y) == TERRAIN_TYPE::kObstacle) {
    return;
  }

  Vector2I v(x, y);
  m_open.push_back(v);
}
