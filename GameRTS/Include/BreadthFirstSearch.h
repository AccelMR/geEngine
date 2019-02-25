#pragma once

#include "GridWalker.h"

class BreadthFirstSearch:
  public GridWalker
{
public:
  BreadthFirstSearch();
  BreadthFirstSearch(RTSTiledMap* pMap);
  ~BreadthFirstSearch();


  bool Init() override;


  void Destroy() override;


  WALKSTATE::E Update() override;


  void Render() override;


  void Reset() override;


  bool weightedGraphSupported() { return false; }


  bool heuristicsSupported() { return false; };

private:

  std::deque<NodeList> m_open;
  Vector2I m_use, m_start, m_end;
  /*RTSTiledMap *m_start, *m_end, *m_use;*/
  RTSTiledMap *m_nodeGrid; //Same size as map grid

protected:
  virtual void visitGridNode(int32 x, int32 y) override;

};