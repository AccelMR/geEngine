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
BreadthFirstSearch::BreadthFirstSearch(RTSTiledMap* pMap) : GridWalker(pMap)
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
  m_nodeGrid = new RTSTiledMap(m_pTiledMap->getMapSize());
  Reset();
  return false;

}

void BreadthFirstSearch::Destroy()
{
  if (nullptr != m_nodeGrid){
    delete m_nodeGrid;
  }
  m_nodeGrid = nullptr;
}

WALKSTATE::E BreadthFirstSearch::Update()
{
  if (m_open.size() > 0)
  {
    m_use = *m_open.front();	
    m_open.pop();
    m_nodeGrid->setVisited(m_use.x, m_use.y, true);


    if (m_use == m_end)
    {
      return WALKSTATE::REACHEDGOAL;	
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

    return WALKSTATE::STILLLOOKING;
  }

  return WALKSTATE::UNABLETOREACHGOAL;
}

void BreadthFirstSearch::Render()
{//TODO: Render
}

void BreadthFirstSearch::Reset()
{
  //Vaciamos el Queue
  while (m_open.size() > 0)
  {
    m_open.pop();
  }

  m_use = Vector2I::ZERO;

  for (int32 i = 0; i < m_pTiledMap->getMapSize().x; i++)
  {
    for (int j = 0; j < m_pTiledMap->getMapSize().y; j++)
    {
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
  m_open.push(&m_start);
}

void BreadthFirstSearch::visitGridNode(int32 x, int32 y)
{
  if (m_nodeGrid->getVisited(x, y) ||
      m_nodeGrid->getType(x,y) == TERRAIN_TYPE::kObstacle) {
    return;
  }

  Vector2I* v = new Vector2I(x, y);
  m_open.push(v);
}
