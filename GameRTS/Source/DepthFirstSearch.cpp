#include "..\Include\DepthFirstSearch.h"

/*
*/
DepthFirstSearch::DepthFirstSearch():
  m_start(nullptr),
  m_end(nullptr),
  m_use(nullptr),
  m_nodeGrid(nullptr)
{
  m_StartPos = Vector2I::ZERO;
  m_EndPos = Vector2I::ZERO;
}

/*
*/
DepthFirstSearch::DepthFirstSearch(RTSTiledMap* pMap) : GridWalker(pMap)
{}

/*
*/
DepthFirstSearch::~DepthFirstSearch()
{
  Destroy();
}


bool DepthFirstSearch::Init()
{
  if (m_nodeGrid) { 
    Destroy();
  }
  m_nodeGrid = new RTSTiledMap(m_pTiledMap->getMapSize());

}

void DepthFirstSearch::Destroy()
{
  if (nullptr != m_nodeGrid){
    delete m_nodeGrid;
  }
  m_nodeGrid = nullptr;
  m_use = nullptr;
}

WALKSTATE::E DepthFirstSearch::Update()
{
  throw std::logic_error("The method or operation is not implemented.");
}

void DepthFirstSearch::Render()
{
  throw std::logic_error("The method or operation is not implemented.");
}

void DepthFirstSearch::Reset()
{
  throw std::logic_error("The method or operation is not implemented.");
}

bool DepthFirstSearch::weightedGraphSupported()
{
  throw std::logic_error("The method or operation is not implemented.");
}

bool DepthFirstSearch::heuristicsSupported()
{
  throw std::logic_error("The method or operation is not implemented.");
}

void DepthFirstSearch::visitGridNode(int32 x, int32 y)
{
  throw std::logic_error("The method or operation is not implemented.");
}
