#pragma once

#include "GridWalker.h"

class sf::RenderTarget;

class AStar :
  public GridWalker
{
public:
  AStar();
  AStar(RTSTiledMap* pMap);
  AStar(RTSTiledMap* pMap, sf::RenderTarget& window);
  ~AStar();


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
    AStar::PriorityQueue(Vector2I& v);

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
  sf::RenderTarget* m_render;

protected:
  virtual void visitGridNode(int32 x, int32 y) override;

};