#pragma once

#include "GridWalker.h"

namespace RTSGame{
class BestFirstSearch :
  public GridWalker
{
public:
  BestFirstSearch();
  BestFirstSearch(RTSTiledMap* map);
  ~BestFirstSearch();

  bool Init() override;


  void Destroy() override;


  WALKSTATE::E Update() override;


  void Render() override;


  void Reset() override;


  bool weightedGraphSupported() {
    return false;
  }

  bool heuristicsSupported() {
    return true;
  }

private:



  List<NodeList> m_open;

  Vector2I m_use, m_start, m_end;
  /*RTSTiledMap *m_start, *m_end, *m_use;*/
  RTSTiledMap *m_nodeGrid; //Same size as map grid

  void
    PriorityQueue(Vector2I& v);

protected:
  virtual void visitGridNode(int32 x, int32 y) override;

};
}