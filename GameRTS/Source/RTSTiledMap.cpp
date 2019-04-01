#include "RTSTiledMap.h"
#include "RTSTexture.h"

#include <geDebug.h>
#include <geColor.h>
namespace RTSGame{
RTSTiledMap::RTSTiledMap() {
  m_mapSize = Vector2I::ZERO;
  m_scrStart = Vector2I::ZERO;
  m_scrEnd = Vector2I::ZERO;
  m_iCamera = Vector2I::ZERO;
  m_fCamera = Vector2::ZERO;
  m_StartMarked = false;
  m_EndMarked = false;
  m_refEndMark = Vector2I::ZERO;
  m_refStartMark = Vector2I::ZERO;
}

RTSTiledMap::RTSTiledMap(sf::RenderTarget* pTarget, const Vector2I& mapSize) {
  init(pTarget, mapSize);
}

RTSTiledMap::RTSTiledMap(const Vector2I & mapSize) :
  m_mapSize(mapSize)
{
  m_mapGrid.resize(mapSize.x * mapSize.y);

  for(std::vector<MapTile>::iterator it = m_mapGrid.begin();
      it != m_mapGrid.end();
      it++)
  {
    it->setVisited(false);
  }
}

RTSTiledMap::~RTSTiledMap() {
  destroy();
}

bool
RTSTiledMap::init(sf::RenderTarget* pTarget, const Vector2I& mapSize) {
  if(m_mapGrid.size())
  {
    destroy();
  }

  m_pTarget = pTarget;

  m_mapGrid.resize(mapSize.x * mapSize.y);
  m_mapSize = mapSize;
  setCameraStartPosition(0, 0);

  m_mapTextures.resize(TERRAIN_TYPE::kNumObjects);
  m_TileMark.resize(PFMARK::NUMOBJ);

  String textureName;
  for(uint32 i = 0; i < TERRAIN_TYPE::kNumObjects; ++i)
  {
#ifdef MAP_IS_ISOMETRIC
    textureName = "Textures/Terrain/iso_terrain_" + toString(i) + ".png";
#else
    textureName = "Textures/Terrain/terrain_" + toString(i) + ".png";
#endif
    m_mapTextures[i].loadFromFile(m_pTarget, textureName);
  }

  //Arrows and flags loader
  for(uint32 i = 1; i < PFMARK::NUMOBJ; ++i)
  {
    textureName = "Textures/Terrain/mark_" + toString(i) + ".png";
    m_TileMark[i].loadFromFile(m_pTarget, textureName);

    if(i == 1 || i == 10)
    {
      continue;
    }
    m_TileMark[i].setOrigin(m_TileMark[i].getWidth() / 2.0f, m_TileMark[i].getHeight() / 2.0f);
    //     m_TileMark[i].setRotation(Degree(45));
    //     m_TileMark[i].setScale(1, 0.5f);

  }

  preCalc();

  return true;
}

void
RTSTiledMap::destroy() {
  m_mapGrid.clear();
  m_mapTextures.clear();
  m_TileMark.clear();

  m_mapSize = Vector2I::ZERO;
  setCameraStartPosition(0, 0);
  preCalc();
}

int8
RTSTiledMap::getCost(const int32 x, const int32 y) const {
  GE_ASSERT((x >= 0) && (x < m_mapSize.x) && (y >= 0) && (y < m_mapSize.y));
  return m_mapGrid[(y*m_mapSize.x) + x].getCost();
}

void
RTSTiledMap::setCost(const int32 x, const int32 y, const int8 cost) {
  GE_ASSERT((x >= 0) && (x < m_mapSize.x) && (y >= 0) && (y < m_mapSize.y));
  m_mapGrid[(y*m_mapSize.x) + x].setCost(cost);
}

void
RTSTiledMap::setMark(const int32 x, const int32 y, const uint8 mark){
  GE_ASSERT((x >= 0) && (x < m_mapSize.x) && (y >= 0) && (y < m_mapSize.y));
  if(mark == PFMARK::START)
  {
    if(m_StartMarked)
    {
      m_mapGrid[(m_refStartMark.y*m_mapSize.x) + m_refStartMark.x].setMark(PFMARK::NONE);
    }
    m_StartMarked = true;
    m_refStartMark = { x,y };
  }
  if(mark == PFMARK::END)
  {
    if(m_EndMarked)
    {
      m_mapGrid[(m_refEndMark.y*m_mapSize.x) + m_refEndMark.x].setMark(PFMARK::NONE);
    }
    m_EndMarked = true;
    m_refEndMark = { x,y };
  }
  m_mapGrid[(y*m_mapSize.x) + x].setMark(mark);
}

void
RTSTiledMap::setVisited(const uint32 x, const uint32 y, const bool visited)
{
  GE_ASSERT((x >= 0) && (x < m_mapSize.x) && (y >= 0) && (y < m_mapSize.y));
  m_mapGrid[(y*m_mapSize.x) + x].setVisited(visited);
}

bool
RTSTiledMap::getVisited(const uint32 x, const uint32 y)
{
  GE_ASSERT((x >= 0) && (x < m_mapSize.x) && (y >= 0) && (y < m_mapSize.y));
  return m_mapGrid[(y*m_mapSize.x) + x].getVisited();
}

uint8
RTSTiledMap::getMark(const int32 x, const int32 y) const
{
  GE_ASSERT((x >= 0) && (x < m_mapSize.x) && (y >= 0) && (y < m_mapSize.y));
  return m_mapGrid[(y*m_mapSize.x) + x].getMark();
}

int8
RTSTiledMap::getType(const int32 x, const int32 y) const {
  GE_ASSERT((x >= 0) && (x < m_mapSize.x) && (y >= 0) && (y < m_mapSize.y));
  return m_mapGrid[(y*m_mapSize.x) + x].getType();
}

void
RTSTiledMap::setType(const int32 x, const int32 y, const uint8 idtype) {
  GE_ASSERT((x >= 0) && (x < m_mapSize.x) && (y >= 0) && (y < m_mapSize.y));
  m_mapGrid[(y*m_mapSize.x) + x].setType(idtype);
}

void
RTSTiledMap::moveCamera(const float dx, const float dy) {
  m_fCamera.x += dx;
  m_fCamera.y += dy;

  m_fCamera.x = Math::clamp(m_fCamera.x, 0.f, static_cast<float>(m_PreCalc_MaxCameraCoord.x));
  m_fCamera.y = Math::clamp(m_fCamera.y, 0.f, static_cast<float>(m_PreCalc_MaxCameraCoord.y));

  setCameraStartPosition(Math::trunc(m_fCamera.x),
                         Math::trunc(m_fCamera.y));
}

void
RTSTiledMap::setCameraStartPosition(const int32 x, const int32 y) {
  Vector2I tmpPos(x, y);
  tmpPos.x = Math::clamp(tmpPos.x, Vector2I::ZERO.x, m_PreCalc_MaxCameraCoord.x);
  tmpPos.y = Math::clamp(tmpPos.y, Vector2I::ZERO.y, m_PreCalc_MaxCameraCoord.y);
  m_iCamera = tmpPos;

#ifdef MAP_IS_ISOMETRIC
  m_PreCalc_ScreenDeface.x = m_scrStart.x + m_PreCalc_MidResolution.x -
    (m_iCamera.x - m_iCamera.y);
  m_PreCalc_ScreenDeface.y = m_scrStart.y + m_PreCalc_MidResolution.y -
    ((m_iCamera.x + m_iCamera.y) >> 1);
#else
  m_PreCalc_ScreenDeface = m_scrStart + m_PreCalc_MidResolution - m_iCamera;
#endif
}

void
RTSTiledMap::getScreenToMapCoords(const int32 scrX,
                                  const int32 scrY,
                                  int32 &mapX,
                                  int32 &mapY) {
#ifdef MAP_IS_ISOMETRIC
  float fscrX = ((float)(scrX - m_PreCalc_ScreenDeface.x) / GameOptions::TILEHALFSIZE.x) - 1;
  float fscrY = ((float)(scrY - m_PreCalc_ScreenDeface.y) / GameOptions::TILEHALFSIZE.y);

  mapX = (Math::trunc(fscrX + fscrY)) >> 1;
  mapY = (Math::trunc(fscrY - fscrX)) >> 1;
#else
  mapX = (scrX - m_PreCalc_ScreenDeface.x) >> GameOptions::BITSHFT_TILESIZE.x;
  mapY = (scrY - m_PreCalc_ScreenDeface.y) >> GameOptions::BITSHFT_TILESIZE.y;
#endif

  mapX = Math::clamp(mapX, 0, m_mapSize.x - 1);
  mapY = Math::clamp(mapY, 0, m_mapSize.y - 1);
}

void
RTSTiledMap::getMapToScreenCoords(const int32 mapX,
                                  const int32 mapY,
                                  int32 &scrX,
                                  int32 &scrY) {
  GE_ASSERT(mapX >= 0 && mapX <= m_mapSize.x && mapY >= 0 && mapY <= m_mapSize.y);

#ifdef MAP_IS_ISOMETRIC
  scrX = (mapX - mapY) << GameOptions::BITSFHT_TILEHALFSIZE.x;
  scrY = (mapX + mapY) << GameOptions::BITSFHT_TILEHALFSIZE.y;

  scrX += m_PreCalc_ScreenDeface.x;
  scrY += m_PreCalc_ScreenDeface.y;
#else
  scrX = (mapX << GameOptions::BITSHFT_TILESIZE.x) + m_PreCalc_ScreenDeface.x;
  scrY = (mapY << GameOptions::BITSHFT_TILESIZE.y) + m_PreCalc_ScreenDeface.y;
#endif
}

void
RTSTiledMap::update(float deltaTime) {
  GE_UNREFERENCED_PARAMETER(deltaTime);
}

void
RTSTiledMap::render() {
  int32 tmpX = 0;
  int32 tmpY = 0;
  int32 tmpTypeTile = 0;
  int32 tmpMarkTile = 0;
  Vector2I clipRect;

  int32 tileIniX = 0, tileIniY = 0;
  int32 tileFinX = 0, tileFinY = 0;

#ifdef MAP_IS_ISOMETRIC
  int32 trashCoord = 0;
  getScreenToMapCoords(m_scrStart.x, m_scrStart.y, tileIniX, trashCoord);
  getScreenToMapCoords(m_scrEnd.x, m_scrEnd.y, tileFinX, trashCoord);

  getScreenToMapCoords(m_scrEnd.x, m_scrStart.y, trashCoord, tileIniY);
  getScreenToMapCoords(m_scrStart.x, m_scrEnd.y, trashCoord, tileFinY);
#else
  getScreenToMapCoords(m_scrStart.x, m_scrStart.y, tileIniX, tileIniY);
  getScreenToMapCoords(m_scrEnd.x, m_scrEnd.y, tileFinX, tileFinY);
#endif
  for(int32 iterX = tileIniX; iterX <= tileFinX; ++iterX)
  {
    for(int32 iterY = tileIniY; iterY <= tileFinY; ++iterY)
    {

      getMapToScreenCoords(iterX, iterY, tmpX, tmpY);
      if(tmpX > m_scrEnd.x ||
         tmpY > m_scrEnd.y ||
         (tmpX + TILESIZE_X) < m_scrStart.x ||
         (tmpY + TILESIZE_X) < m_scrStart.y)
      {
        continue;
      }
      /*      auto tileRef = m_mapGrid[(iterY*m_mapSize.x) + iterX];*/
      tmpTypeTile = m_mapGrid[(iterY*m_mapSize.x) + iterX].getType();

      RTSTexture& refTexture = m_mapTextures[tmpTypeTile];

      clipRect.x = (iterX << GameOptions::BITSHFT_TILESIZE.x) % refTexture.getWidth();
      clipRect.y = (iterY << GameOptions::BITSHFT_TILESIZE.y) % refTexture.getHeight();

      refTexture.setPosition(tmpX, tmpY);

      //Color randColor;
      //randColor = randColor.makeRandomColor();
      //refTexture.setColor(randColor.r, randColor.g, randColor.b, randColor.a);

      refTexture.setSrcRect(clipRect.x, clipRect.y, TILESIZE_X, TILESIZE_Y);
      refTexture.draw();
    }
  }

  if(GameOptions::s_MapShowGrid)
  {
    FrameVector<sf::Vertex> gridLines;
    gridLines.reserve(((tileFinX - tileIniX) + (tileFinY - tileIniY) + 4) << 1);

    sf::Color gridColor(255, 0, 0, 255);

    int32 tmpX2 = 0, tmpY2 = 0;
    for(int32 iterX = tileIniX; iterX <= tileFinX + 1; ++iterX)
    {
      getMapToScreenCoords(iterX, tileIniY, tmpX, tmpY);
      getMapToScreenCoords(iterX, tileFinY, tmpX2, tmpY2);
#ifdef MAP_IS_ISOMETRIC
      gridLines.push_back(sf::Vertex(
        sf::Vector2f(static_cast<float>(tmpX + GameOptions::TILEHALFSIZE.x),
                     static_cast<float>(tmpY)),
        gridColor));

      gridLines.push_back(sf::Vertex(
        sf::Vector2f(static_cast<float>(tmpX2),
                     static_cast<float>(tmpY2 + GameOptions::TILEHALFSIZE.y)),
        gridColor));
#else
      gridLines.push_back(sf::Vertex(
        sf::Vector2f(static_cast<float>(tmpX), static_cast<float>(tmpY)),
        gridColor));

      gridLines.push_back(sf::Vertex(sf::Vector2f(static_cast<float>(tmpX2),
                                                  static_cast<float>(tmpY2 + TILESIZE_Y)),
                                     gridColor));
#endif
    }

    for(int32 iterY = tileIniY; iterY <= tileFinY + 1; ++iterY)
    {
      getMapToScreenCoords(tileIniX, iterY, tmpX, tmpY);
      getMapToScreenCoords(tileFinX, iterY, tmpX2, tmpY2);
#ifdef MAP_IS_ISOMETRIC
      gridLines.push_back(sf::Vertex(
        sf::Vector2f(static_cast<float>(tmpX + GameOptions::TILEHALFSIZE.x),
                     static_cast<float>(tmpY)),
        gridColor));

      gridLines.push_back(sf::Vertex(
        sf::Vector2f(static_cast<float>(tmpX2 + TILESIZE_X),
                     static_cast<float>(tmpY2 + GameOptions::TILEHALFSIZE.y)),
        gridColor));
#else
      gridLines.push_back(sf::Vertex(
        sf::Vector2f(static_cast<float>(tmpX), static_cast<float>(tmpY)),
        gridColor));

      gridLines.push_back(sf::Vertex(sf::Vector2f(static_cast<float>(tmpX2 + TILESIZE_X),
                                                  static_cast<float>(tmpY2)),
                                     gridColor));
#endif
    }

    m_pTarget->draw(&gridLines[0], gridLines.size(), sf::Lines);
  }

#ifdef MAP_IS_ISOMETRIC
  getScreenToMapCoords(m_scrStart.x, m_scrStart.y, tileIniX, trashCoord);
  getScreenToMapCoords(m_scrEnd.x, m_scrEnd.y, tileFinX, trashCoord);

  getScreenToMapCoords(m_scrEnd.x, m_scrStart.y, trashCoord, tileIniY);
  getScreenToMapCoords(m_scrStart.x, m_scrEnd.y, trashCoord, tileFinY);
#else
  getScreenToMapCoords(m_scrStart.x, m_scrStart.y, tileIniX, tileIniY);
  getScreenToMapCoords(m_scrEnd.x, m_scrEnd.y, tileFinX, tileFinY);
#endif
  for(int32 iterX = tileIniX; iterX <= tileFinX; ++iterX)
  {
    for(int32 iterY = tileIniY; iterY <= tileFinY; ++iterY)
    {

      getMapToScreenCoords(iterX, iterY, tmpX, tmpY);
      if(tmpX > m_scrEnd.x ||
         tmpY > m_scrEnd.y ||
         (tmpX + TILESIZE_X) < m_scrStart.x ||
         (tmpY + TILESIZE_X) < m_scrStart.y)
      {
        continue;
      }

      tmpMarkTile = m_mapGrid[(iterY*m_mapSize.x) + iterX].getMark();

      if(tmpMarkTile == PFMARK::NONE)
      {
        continue;
      }

      RTSTexture& refMark = m_TileMark[tmpMarkTile];

      if(tmpMarkTile == PFMARK::END || tmpMarkTile == PFMARK::START)
      {
        int32 y = tmpY + (TILESIZE_Y >> 1) - refMark.getHeight();
        int32 x = tmpX + (TILESIZE_X >> 1);
        refMark.setPosition(x, y);
        refMark.draw();
      }
      else
      {
        int32 y = tmpY + (TILESIZE_Y >> 1) /*- refMark.getHeight()*/;
        int32 x = tmpX + (TILESIZE_X >> 1);
        refMark.setPosition(x, y);
        refMark.draw();
      }
    }
  }

}

RTSTiledMap::MapTile::MapTile() {
  m_idType = 1;
  m_cost = 1;
  m_visited = false;
  m_pfMark = PFMARK::NONE;
}

RTSTiledMap::MapTile::MapTile(const int8 idType, const int8 cost) {
  m_idType = idType;
  m_cost = cost;
  m_visited = false;
  m_pfMark = PFMARK::NONE;
}

RTSTiledMap::MapTile::MapTile(const MapTile& copy) {
  m_idType = copy.m_idType;
  m_cost = copy.m_cost;
  m_pfMark = PFMARK::NONE;
  m_visited = false;
}

RTSTiledMap::MapTile&
RTSTiledMap::MapTile::operator=(const MapTile& rhs) {
  m_idType = rhs.m_idType;
  m_cost = rhs.m_cost;
  return *this;
}

bool
RTSTiledMap::loadFromImageFile(sf::RenderTarget* pTarget, String fileName) {
  sf::Image image;

  if(!image.loadFromFile(fileName.c_str()))
  {
    LOGWRN("File not found: " + fileName);
    return false;
  }

  if(!init(pTarget, Vector2I(image.getSize().x, image.getSize().y)))
  {
    LOGERR("Failed initializing map data");
    return false;
  }

  //With this library, the pixels always are 4 bytes in length
  int32 lineBytes = image.getSize().x * 4;
  const uint8* pPixeles = reinterpret_cast<const uint8*>(image.getPixelsPtr());

  for(int32 tmpY = 0; tmpY < m_mapSize.y; ++tmpY)
  {
    for(int32 tmpX = 0; tmpX < m_mapSize.x; ++tmpX)
    {
      uint8 tipoTerreno = TERRAIN_TYPE::kObstacle;
      Color actualColor(
        pPixeles[(tmpY * lineBytes) + (tmpX * 4) + 0],
        pPixeles[(tmpY * lineBytes) + (tmpX * 4) + 1],
        pPixeles[(tmpY * lineBytes) + (tmpX * 4) + 2],
        pPixeles[(tmpY * lineBytes) + (tmpX * 4) + 3]);

      if(Color::Blue == actualColor)
      {
        tipoTerreno = TERRAIN_TYPE::kWater;
      }
      else if(Color::Green == actualColor)
      {
        tipoTerreno = TERRAIN_TYPE::kGrass;
      }
      else if(Color::Yellow == actualColor)
      {
        tipoTerreno = TERRAIN_TYPE::kMarsh;
      }

      setType(tmpX, tmpY, tipoTerreno);
    }
  }

  return true;
}

bool
RTSTiledMap::saveToImageFile(sf::RenderTarget*, String) {
  return false;
}
}