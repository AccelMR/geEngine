#include "..\Include\GridWalker.h"
namespace RTSGame{
GridWalker::GridWalker(){}

GridWalker::~GridWalker(){}

Vector<Vector2I>
GridWalker::BackTracing()
{
  Vector<Vector2I> path;

  Vector2I v = m_EndPos;
  NodeList temp = m_close.back();

  int32 i = m_close.size() - 1;
  while(temp.parent != m_StartPos)
  {
    while(temp.position != v)
    {
      temp = m_close[--i];
    }
    path.push_back(temp.position);
    v = temp.parent;
  }
  path.push_back(m_StartPos);
  /*ClearClose();*/
  return path;
}

void GridWalker::ClearClose()
{
  for(Vector<NodeList>::iterator it = m_close.begin(); it != m_close.end(); ++it)
  {
    m_pTiledMap->setVisited(it->position.x, it->position.y, false);

    if(m_pTiledMap->getMark(it->position.x, it->position.y) == PFMARK::START ||
       m_pTiledMap->getMark(it->position.x, it->position.y) == PFMARK::END)
    {
      continue;
    }

    m_pTiledMap->setMark(it->position.x, it->position.y, PFMARK::NONE);
  }
  m_close.clear();
}
}
