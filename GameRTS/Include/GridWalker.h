#pragma once

#include "RTSTiledMap.h"

namespace WALKSTATE
{
  typedef enum E
  {
    STILLLOOKING,
    REACHEDGOAL,
    UNABLETOREACHGOAL
  };
}

namespace TYPE_PATH_FINDER
{
  enum E
  {
    DEPTHFIRSTSERCH = 0,
    BREATHFIRSTSERCH,
    BESTFIRSTSEARCH,
    DIJSTRA,
    ASTAR,
    NUMBOJ
  };

  static Vector<String> ES = {
    "Depth First Search",
    "Breath First Search",
    "Best First Search",
    "Dijstra",
    "A*"
  };
}

class GridWalker
{
public:
  GridWalker();
  GridWalker(RTSTiledMap* pMap) { m_pTiledMap = pMap; }
  virtual ~GridWalker();

  virtual bool Init() { return false; }						//Función de inicialización de la clase (por default en este aspecto regresamos false porque no estamos haciendo nada)
  virtual void Destroy() = 0;											//Destruye los objetos de la clase
  virtual WALKSTATE::E Update() = 0;						  //Actualiza el estado del pathfinding (puede hacerse por pasos o recursivamente)
  virtual void Render() = 0;											//Renderea la información necesaria para su uso en pathfinding
  virtual void Reset() = 0;												//Reinicializa las variables de esta clase para su uso en un nuevo cálculo

  virtual bool weightedGraphSupported() { return false; }				//Indica si este Walker soporta la asignación y el uso de pesos a los nodos del graph
  virtual bool heuristicsSupported() { return false; }					//Indica si este Walker soporta heuristicas

  void setTiledMap(RTSTiledMap *pMap) { m_pTiledMap = pMap; }			//Establecemos un puntero al mapa que será utilizado para todos los cálculos
  RTSTiledMap *getMapGrid() { return m_pTiledMap; }					//Regresa el puntero al mapa que se está utilizando para todos los cálculos

  void setStartPosition(const int32 x, const int32 y) { m_StartPos.x = x; m_StartPos.y = y; }	//Esta función se utiliza para establecer el punto inicial de búsqueda
  void setEndPosition(const int32 x, const int32 y) { m_EndPos.x = x; m_EndPos.y = y; }			//Esta función se utiliza para establecer el punto final de la búsqueda

  void getStartPosition(int32 & x, int32 & y) { x = m_StartPos.x; y = m_StartPos.y; }				//Función de ayuda para obtener el punto inicial de búsqueda
  void getEndPosition(int32 & x, int32 & y) { x = m_EndPos.x; y = m_EndPos.y; }						//Función de ayuda para obtener el punto final de búsqueda

  WALKSTATE::E GetState() { return m_currentState; }

  Vector<Vector2I> BackTracing();

protected:
  virtual void visitGridNode(int32 x, int32 y) = 0;					//Marca un nodo de mapa como visitado (esto lo procesa según el algoritmo utilizado)

  void ClearClose();

  /************************************************************************************************************************/
  /* Declaración de variables miembro de la clase                                 										*/
  /************************************************************************************************************************/
protected:

  struct CloseList
  {
    Vector2I position;
    Vector2I parent;
  };
  struct OpenList
  {
    Vector2I position;
    Vector2I parent;
  };

  

  List<CloseList> m_close;
  WALKSTATE::E m_currentState;
  RTSTiledMap *m_pTiledMap;									//Puntero al mapa que estamos usando para calcular
  Vector2I m_StartPos, m_EndPos;

};