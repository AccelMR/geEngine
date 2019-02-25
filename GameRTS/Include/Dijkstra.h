#pragma once

#include "GridWalker.h"

class Dijkstra :
  public GridWalker
{
public:
  Dijkstra();
  Dijkstra(RTSTiledMap* pMap);
  ~Dijkstra();


  bool
  Init() override;


  void 
  Destroy() override;


  WALKSTATE::E Update() override;


  void 
  Render() override;


  void 
  Reset() override;


  bool 
  weightedGraphSupported() { return false; }


  bool 
  heuristicsSupported() { return false; };
  
  void
  Dijkstra::PriorityQueue(Vector2I& v);

  Vector<Vector2I> 
  BackTracing();

private:
  struct NodeListD
  {
    Vector2I position;
    Vector2I parent;
    uint32 weight;
  
  };

  List<NodeListD> m_open;
  Vector<NodeListD> m_closeD;
  Vector2I m_use, m_start, m_end;
  /*RTSTiledMap *m_start, *m_end, *m_use;*/
  RTSTiledMap *m_nodeGrid; //Same size as map grid

protected:
  virtual void visitGridNode(int32 x, int32 y) override;

};