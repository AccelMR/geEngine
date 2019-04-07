#include "..\Include\Dijkstra.h"

#include <iostream>
namespace RTSGame{
/*
*/
Dijkstra::Dijkstra() :
  m_nodeGrid(nullptr)
{
  m_StartPos = Vector2I::ZERO;
  m_EndPos = Vector2I::ZERO;
}

Dijkstra::Dijkstra(RTSTiledMap * pMap, sf::RenderTarget* window) :
  GridWalker(pMap),
  m_nodeGrid(nullptr),
  m_render(window)
{
}

/*
*/
Dijkstra::Dijkstra(RTSTiledMap* pMap) :
  GridWalker(pMap),
  m_nodeGrid(nullptr)
{}

/*
*/
Dijkstra::~Dijkstra()
{
  Destroy();
  if(nullptr != m_arialFont)
  {
    delete m_arialFont;
    m_arialFont = nullptr;
  }
}

bool Dijkstra::Init()
{
  if(m_nodeGrid)
  {
    Destroy();
  }
  m_nodeGrid = m_pTiledMap;

  m_arialFont = ge_new<sf::Font>();
  if(nullptr == m_arialFont)
  {
    GE_EXCEPT(InvalidStateException, "Couldn't create a Font");
  }


  if(!m_arialFont->loadFromFile("Fonts/arial.ttf"))
  {
    GE_EXCEPT(FileNotFoundException, "Arial font not found");
  }
  return false;

}

void Dijkstra::Destroy()
{
  //   if (nullptr != m_nodeGrid){
  //     delete m_nodeGrid;
  //   }
  m_nodeGrid = nullptr;
}

WALKSTATE::E Dijkstra::Update()
{
  if(m_open.size() > 0)
  {
    m_use = m_open.front().position;
    m_closeD.push_back({ m_use,m_open.front().parent, m_open.front().weight });

    m_open.pop_front();
    m_nodeGrid->setVisited(m_use.x, m_use.y, true);

    if(m_use == m_end)
    {
      m_currentState = WALKSTATE::REACHEDGOAL;
      return m_currentState;
    }
    if(m_use != m_start)
    {
      m_nodeGrid->setMark(m_use.x, m_use.y, PFMARK::N);
    }

    int32 x, y;
    x = m_use.x + 1;
    y = m_use.y;
    if(m_use.x < (m_pTiledMap->getMapSize().x - 1))
    {
      visitGridNode(x, y);
    }

    x = m_use.x + 1;
    y = m_use.y + 1;
    if(m_use.x < (m_pTiledMap->getMapSize().x - 1) &&
       m_use.y < (m_pTiledMap->getMapSize().y - 1))
    {
      visitGridNode(x, y);
    }

    x = m_use.x;
    y = m_use.y + 1;
    if(m_use.y < (m_pTiledMap->getMapSize().y - 1))
    {
      visitGridNode(x, y);
    }

    x = m_use.x - 1;
    y = m_use.y + 1;
    if(m_use.y < (m_pTiledMap->getMapSize().y - 1) && m_use.x > 0)
    {
      visitGridNode(x, y);
    }

    x = m_use.x - 1;
    y = m_use.y;
    if(m_use.x > 0)
    {
      visitGridNode(x, y);
    }

    x = m_use.x - 1;
    y = m_use.y - 1;
    if(m_use.x > 0 && m_use.y > 0)
    {
      visitGridNode(x, y);
    }

    x = m_use.x;
    y = m_use.y - 1;
    if(m_use.y > 0)
    {
      visitGridNode(x, y);
    }

    x = m_use.x + 1;
    y = m_use.y - 1;
    if(m_use.y > 0 && m_use.x < (m_pTiledMap->getMapSize().x - 1))
    {
      visitGridNode(x, y);
    }

    m_currentState = WALKSTATE::STILLLOOKING;
    return m_currentState;
  }

  m_currentState = WALKSTATE::UNABLETOREACHGOAL;
  return m_currentState;

}

void Dijkstra::Render()
{
  sf::Text text;
  /*int32 x, y = 0;*/

  text.setCharacterSize(12);
  text.setFillColor(sf::Color::Black);
  text.setFont(*m_arialFont);

//   int32 tmpX = 0;
//   int32 tmpY = 0;
//   int32 tmpTypeTile = 0;
  /*int32 tmpMarkTile = 0;*/
  /*Vector2I clipRect;*/

//   int32 tileIniX = 0; 
//   int32 tileFinX = 0; 
  
  int32 tileIniY = 0;  
  int32 tileFinY = 0;

#ifdef MAP_IS_ISOMETRIC
  int32 trashCoord = 0;
  //      m_pTiledMap->getScreenToMapCoords(m_scrStart.x, m_scrStart.y, tileIniX, trashCoord);
  //      m_pTiledMap->getScreenToMapCoords(m_scrEnd.x, m_scrEnd.y, tileFinX, trashCoord);

  m_pTiledMap->getScreenToMapCoords(0, 0, trashCoord, tileIniY);
  m_pTiledMap->getScreenToMapCoords(1920, 1080, trashCoord, tileFinY);
#else
  m_pTiledMap->getScreenToMapCoords(0, 0, tileIniX, tileIniY);
  m_pTiledMap->getScreenToMapCoords(1920, 1080, tileFinX, tileFinY);
#endif
  //      for (int32 i = 0; i < m_closeD.size(); ++i)
  //      {
  //        if (m_closeD[i].position.x < tileIniX || m_closeD[i].position.x > tileFinX ||
  //            m_closeD[i].position.y < tileIniY || m_closeD[i].position.y > tileFinY)
  //        {
  //          continue;
  //        }
  //         m_pTiledMap->getMapToScreenCoords(m_closeD[i].position.x,
  //                                           m_closeD[i].position.y, x, y);
  //         text.setPosition(x, y);
  //         text.setString(toString(m_closeD[i].weight).c_str());
  //         m_render->draw(text);
  //      }
}

void Dijkstra::Reset()
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

void Dijkstra::visitGridNode(int32 x, int32 y)
{
  if(m_nodeGrid->getVisited(x, y) ||
     m_nodeGrid->getType(x, y) == TERRAIN_TYPE::kObstacle)
  {
    return;
  }

  Vector2I v(x, y);
  PriorityQueue(v);
}

void
Dijkstra::PriorityQueue(Vector2I& v)
{
  uint32 weight = m_pTiledMap->getCost(v.x, v.y) + m_closeD.back().weight;

  for(std::list<NodeListD>::iterator it = m_open.begin(); it != m_open.end(); ++it)
  {
    //     for (Vector<NodeListD>::iterator it2 = m_closeD.begin(); it2 != m_closeD.end(); ++it2)
    //     {
    //       if (it2->position == m_use)
    //       {
    //         weight += it->weight;
    //         continue;
    //       }
    //     }
    if(it->position == v)
    {
      return;
    }

    if(weight < it->weight)
    {
      m_open.insert(it, { v, m_use,weight });
      return;
    }
  }
  m_open.push_back({ v, m_use,weight });
}

Vector<Vector2I> Dijkstra::BackTracing()
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