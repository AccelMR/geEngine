#pragma once

#include <gePlatformUtility.h>
#include <geVector2.h>
#include <geVector2I.h>

#include <SFML/Graphics.hpp>

#include "RTSConfig.h"


using namespace geEngineSDK;

class RTSTexture;

namespace TERRAIN_TYPE {
  enum E {
    kWater = 0,
    kGrass,
    kMarsh,
    kObstacle,
    kNumObjects, //How many terrains we have
  };

  static Vector<String> ES = {
    "Water",
    "Grass" ,
    "Marsh",
    "Obstacle",
  };
}

 enum PFMARK
{
  NONE=0,
  START,
  N,
  NE,
  E,
  SE,
  S,
  SO,
  O,
  NO,
  END,
  NUMOBJ
};

class RTSTiledMap
{
  class MapTile
  {
   public:
    MapTile();
    MapTile(const int8 idType, const int8 cost);
    MapTile(const MapTile& copy);
    
    MapTile&
    operator=(const MapTile& rhs);

    FORCEINLINE uint8
    getType() const {
      return m_idType;
    }

    void
    setType(const int8 idType) {
      m_idType = idType;
    }

    FORCEINLINE int8
    getCost() const {
      return m_cost;
    }

    void
    setCost(const int8 cost) {
      m_cost = cost;
    }

    void
    setMark(const uint8 mark) {
      m_pfMark = mark;
    }

    uint8
    getMark() const {
      return m_pfMark;
    }

    void
    setVisited(bool visited) {
      m_visited = visited;
    }

    bool
    getVisited() { 
      return m_visited; 
    }

   private:
    uint8 m_idType;
    int8 m_cost;
    uint8 m_pfMark;
    bool m_visited;
  };

 public:
  RTSTiledMap();
  RTSTiledMap(sf::RenderTarget* pTarget, const Vector2I& mapSize);
  RTSTiledMap( const Vector2I& mapSize);
  ~RTSTiledMap();

 public:
  bool
  init(sf::RenderTarget* pTarget, const Vector2I& mapSize);

  void
  destroy();

  void
  update(float deltaTime);
  
  void
  render();

  Vector2I
  getMapSize() const {
    return m_mapSize;
  }

  bool
  loadFromImageFile(sf::RenderTarget* pTarget, String fileName);

  bool
  saveToImageFile(sf::RenderTarget* pTarget, String fileName);

  int8
  getCost(const int32 x, const int32 y) const;

  void
  setCost(const int32 x, const int32 y, const int8 cost);

  void
  setMark(const uint8 x, const uint8 y, const uint8 mark);

  void
  setVisited(const uint8 x, const uint8 y, const bool visited);

  bool
  getVisited(const uint8 x, const uint8 y);

  uint8
  getMark(const int32 x, const int32 y) const;

  int8
  getType(const int32 x, const int32 y) const;

  void
  setType(const int32 x, const int32 y, const uint8 idtype);

  void
  setStart(const uint32 x, const uint32 y) {
    m_scrStart.x = x;
    m_scrStart.y = y;
    preCalc();
  }

  void
  getStart(uint32 &x, uint32 &y) const {
    x = m_scrStart.x;
    y = m_scrStart.y;
  }

  void
  setEnd(const uint32 x, const uint32 y) {
    m_scrEnd.x = x;
    m_scrEnd.y = y;
    preCalc();
  }

  void
  getEnd(uint32 &x, uint32 &y) const {
    x = m_scrEnd.x;
    y = m_scrEnd.y;
  }

  void
  preCalc() {
    m_PreCalc_MidResolution = (m_scrEnd - m_scrStart) / 2;
#ifdef MAP_IS_ISOMETRIC
    m_PreCalc_MaxCameraCoord.x = m_mapSize.x * GameOptions::TILEHALFSIZE.x;
#else
    m_PreCalc_MaxCameraCoord.x = m_mapSize.x * TILESIZE_X;
#endif
    m_PreCalc_MaxCameraCoord.y = m_mapSize.y * TILESIZE_Y;
  }

  void
  moveCamera(const float dx, const float dy);

  void
  setCameraStartPosition(const int32 x, const int32 y);

  void
  getCameraStartPosition(int32 &x, int32 &y) const {
    x = m_iCamera.x;
    y = m_iCamera.y;
  }

  void
  getScreenToMapCoords(const int32 scrX,
                       const int32 scrY,
                       int32 &mapX,
                       int32 &mapY);

  void
  getMapToScreenCoords(const int32 mapX,
                       const int32 mapY,
                       int32 &scrX,
                       int32 &scrY);

 private:
  Vector2I m_mapSize;
  Vector<MapTile> m_mapGrid;
  Vector<RTSTexture> m_mapTextures;
  Vector<RTSTexture> m_TileMark;

  Vector2I m_iCamera;
  Vector2 m_fCamera;

  Vector2I m_scrStart;
  Vector2I m_scrEnd;

  Vector2I m_PreCalc_MidResolution;
  Vector2I m_PreCalc_MaxCameraCoord;
  Vector2I m_PreCalc_ScreenDeface;

  sf::RenderTarget* m_pTarget;

  Vector2I m_refEndMark, m_refStartMark;
  bool m_StartMarked, m_EndMarked;
};