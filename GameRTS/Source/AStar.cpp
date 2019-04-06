#include "..\Include\AStar.h"

#include <iostream>

#include <SFML/Graphics.hpp>
namespace RTSGame {

/*
*/
AStar::AStar() :
  m_nodeGrid(nullptr)
{
  m_StartPos = Vector2I::ZERO;
  m_EndPos = Vector2I::ZERO;
}

/*
*/
AStar::AStar(RTSTiledMap* pMap) :
  GridWalker(pMap),
  m_nodeGrid(nullptr)
{}

AStar::AStar(RTSTiledMap * pMap, sf::RenderTarget& window) :
  GridWalker(pMap),
  m_nodeGrid(nullptr),
  m_render(&window)
{
}

/*
*/
AStar::~AStar()
{
  Destroy();
}

bool AStar::Init()
{
  if(m_nodeGrid)
  {
    Destroy();
  }
  m_nodeGrid = m_pTiledMap;
  return false;

}

void AStar::Destroy()
{
  //   if (nullptr != m_nodeGrid){
  //     delete m_nodeGrid;
  //   }
  m_nodeGrid = nullptr;
}

WALKSTATE::E 
AStar::Update()
{
  while (m_currentState == WALKSTATE::STILLLOOKING)
  {
//     if (m_open.size() > 0)
//     {
      m_use = m_open.front().position;
      m_closeD.push_back({ m_use,m_open.front().parent, m_open.front().weight });

      m_open.pop_front();
      m_nodeGrid->setVisited(m_use.x, m_use.y, true);

      if (m_use == m_end)
      {
        m_currentState = WALKSTATE::REACHEDGOAL;
        break;
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
    //}

    /*m_currentState = WALKSTATE::UNABLETOREACHGOAL;*/
  }
  return WALKSTATE::REACHEDGOAL;
}

void AStar::Render()
{//TODO: Render
}

void AStar::Reset()
{
  m_open.clear();

  m_use = Vector2I::ZERO;

  /*ClearClose();*/
  for(Vector<NodeListD>::iterator it = m_closeD.begin(); it != m_closeD.end(); ++it)
  {
    m_pTiledMap->setVisited(it->position.x, it->position.y, false);

    if(m_pTiledMap->getMark(it->position.x, it->position.y) == PFMARK::START ||
       m_pTiledMap->getMark(it->position.x, it->position.y) == PFMARK::END)
    {
      continue;
    }

    m_pTiledMap->setMark(it->position.x, it->position.y, PFMARK::NONE);
  }
  m_closeD.clear();

  int x, y;
  getStartPosition(x, y);
  m_start = m_StartPos;
  m_nodeGrid->setVisited(m_start.x, m_start.y, true);

  //Obtenemos el punto final, obtenemos el nodo y lo marcamos como el nodo final
  getEndPosition(x, y);
  m_end = m_EndPos;

  //Agregamos el nodo inicial a la lista abierta
  m_open.push_back({ m_start,m_start,0 });
  m_currentState = WALKSTATE::STILLLOOKING;
}

void AStar::visitGridNode(int32 x, int32 y)
{
  if(m_nodeGrid->getVisited(x, y) ||
     m_nodeGrid->getType(x, y) == TERRAIN_TYPE::kObstacle)
  {
    return;
  }

  Vector2I v(x, y);

  for(std::list<NodeListD>::iterator it = m_open.begin(); it != m_open.end(); ++it)
  {
    if(it->position == v)
    {
      return;
    }
  }

  PriorityQueue(v);
}

void
AStar::PriorityQueue(Vector2I& v)
{
  uint32 weight = m_pTiledMap->getCost(v.x, v.y);
  uint32 distance = v.manhattanDist(m_end);


  for(std::list<NodeListD>::iterator it = m_open.begin(); it != m_open.end(); ++it)
  {
    weight += m_closeD.back().weight;
    if(it->position == v)
    {
      return;
    }

    uint32 otherWeight = it->weight;
    uint32 otherDistance = it->position.manhattanDist(m_end);

    if(weight + distance < otherWeight + otherDistance)
    {
      m_open.insert(it, { v, m_use,weight });
      return;
    }
  }
  m_open.push_back({ v, m_use,weight });
}

Vector<Vector2I> AStar::BackTracing()
{
  Vector<Vector2I> path;

  Vector2I v = m_EndPos;
  NodeListD temp = m_closeD.back();

  int32 i = m_closeD.size() - 1;
  while(temp.parent != m_StartPos)
  {
    while(temp.position != v)
    {
      temp = m_closeD[--i];
    }
    path.push_back(temp.position);
    v = temp.parent;
  }

  /*ClearClose();*/
  path.push_back(m_start);
  return path;
}
}
