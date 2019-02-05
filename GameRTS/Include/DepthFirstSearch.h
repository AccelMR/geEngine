#pragma once

#include "GridWalker.h"

class DepthFirstSearch:
  public GridWalker
{
public:
  DepthFirstSearch();
  DepthFirstSearch(RTSTiledMap* pMap);
  ~DepthFirstSearch();


  bool Init() override;


  void Destroy() override;


  WALKSTATE::E Update() override;


  void Render() override;


  void Reset() override;


  bool weightedGraphSupported() { return false; }


  bool heuristicsSupported() { return false; };

private:
  std::queue<RTSTiledMap*> m_open;
  RTSTiledMap *m_start, *m_end, *m_use;
  RTSTiledMap *m_nodeGrid; //Same size as map grid

protected:
  virtual void visitGridNode(int32 x, int32 y) override;

};