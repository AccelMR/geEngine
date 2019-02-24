#include "..\Include\GridWalker.h"

GridWalker::GridWalker(){}

GridWalker::~GridWalker(){}

Vector<Vector2I>
GridWalker::BackTracing()
{
  Vector<Vector2I> path;

  Vector2I v = m_EndPos;

  Vector2I temp = m_close.back().position;

  while (m_close.back().position != m_StartPos)
  {
    while (m_close.back().position != v)
    {
      m_pTiledMap->setMark(m_close.back().position.x, m_close.back().position.y, PFMARK::NONE);
      m_pTiledMap->setVisited(m_close.back().position.x, m_close.back().position.y, false);
      m_close.pop_back();
    }
    path.push_back(m_close.back().position);
    v = m_close.back().parent;
  }
  /*ClearClose();*/
  return path;
}

void GridWalker::ClearClose()
{
  for (std::list<CloseList>::iterator it = m_close.begin(); it != m_close.end(); ++it)
  {
    m_pTiledMap->setVisited(it->position.x, it->position.y, false);

    if (m_pTiledMap->getMark(it->position.x, it->position.y) == PFMARK::START ||
      m_pTiledMap->getMark(it->position.x, it->position.y) == PFMARK::END)
    {
      continue;
    }

    m_pTiledMap->setMark(it->position.x, it->position.y, PFMARK::NONE);
  }
  m_close.clear();
}
