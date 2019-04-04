#include <gePrerequisitesUtil.h>

#if GE_PLATFORM == GE_PLATFORM_WIN32
# include <Win32/geWin32Windows.h>
#endif

#include <geRTTIPlainField.h>
#include <geException.h>
#include <geMath.h>

#include <geCrashHandler.h>
#include <geDynLibManager.h>
#include <geFileSystem.h>
#include <geTime.h>
#include <geUnicode.h>

#include <SFML/Graphics.hpp>
#include <imgui-sfml.h>
#include <commdlg.h>
#include <imgui.h>

#include "RTSConfig.h"
#include "RTSApplication.h"
#include "RTSTiledMap.h"
#include "RTSUnitType.h"
#include "RTSUnit.h"
#include "GridWalker.h"
#include "RTSWorld.h"

namespace RTSGame{

void
loadMapFromFile(RTSApplication* pApp);

void
mainMenu(RTSApplication* pApp);

//Terrain Modifier
int32 g_iTerrainSelected = 0;
int32 g_iStartSelection = 0;
int32 g_iPathFinders = 0;
int32 g_iUnitType = 0;

RTSApplication::RTSApplication()
  : m_window(nullptr),
  m_fpsTimer(0.0f),
  m_fpsCounter(0.0f),
  m_framesPerSecond(0.0f),
  m_startArea(false)
{}

RTSApplication::~RTSApplication() {}

int32
RTSApplication::run() {
  CrashHandler::startUp();
  DynLibManager::startUp();
  Time::startUp();
  GameOptions::startUp();

  __try
  {
    initSystems();
    gameLoop();
    destroySystems();
  }
  __except(g_crashHandler().reportCrash(GetExceptionInformation())) {
    PlatformUtility::terminate(true);
  }

  GameOptions::shutDown();
  Time::shutDown();
  DynLibManager::shutDown();
  CrashHandler::shutDown();

  return 0;
}

void
RTSApplication::initSystems() {
  if(nullptr != m_window)
  {  //Window already initialized
    return; //Shouldn't do anything
  }

  //Create the application window
  m_window = ge_new<sf::RenderWindow>(sf::VideoMode(GameOptions::s_Resolution.x,
                                                    GameOptions::s_Resolution.y),
                                      "RTS Game",
                                      sf::Style::Fullscreen);
  if(nullptr == m_window)
  {
    GE_EXCEPT(InvalidStateException, "Couldn't create Application Window");
  }

  m_arialFont = ge_new<sf::Font>();
  if(nullptr == m_arialFont)
  {
    GE_EXCEPT(InvalidStateException, "Couldn't create a Font");
  }


  if(!m_arialFont->loadFromFile("Fonts/arial.ttf"))
  {
    GE_EXCEPT(FileNotFoundException, "Arial font not found");
  }

  //m_window->setVerticalSyncEnabled(true);

  initGUI();
}

void
RTSApplication::initGUI() {
  ImGui::SFML::Init(*m_window);
}

void
RTSApplication::destroySystems() {
  ImGui::SFML::Shutdown();

  if(nullptr != m_window)
  {
    m_window->close();
    ge_delete(m_window);
  }

  if(nullptr != m_arialFont)
  {
    ge_delete(m_arialFont);
  }
  ge_delete(m_cursorTexture);
}

void
RTSApplication::gameLoop() {
  if(nullptr == m_window)
  {  //Windows not yet initialized
    return; //Shouldn't do anything
  }

  postInit();

  // Load image and create sprite
  m_cursorTexture = ge_new<RTSTexture>();
  m_cursorTexture->loadFromFile(m_window, "Textures/GUI/hand.png");
  m_cursorTexture->setScale(.10, .10);
  m_cursorTexture->setOrigin(m_cursorTexture->getWidth() / 8.f,
                             m_cursorTexture->getHeight() / 15.f);

  if (!m_music.openFromFile("Music/song.ogg"))
  {
    return;
  }

  while(m_window->isOpen())
  {

    auto map = m_gameWorld.getTiledMap();
    int32 tileX, tileY;
    sf::Vector2i mousePos = sf::Mouse::getPosition();
    map->getScreenToMapCoords(mousePos.x, mousePos.y, tileX, tileY);
    sf::Event event;


    while(m_window->pollEvent(event))
    {
      ImGui::SFML::ProcessEvent(event);

      switch(event.type)
      {
      case sf::Event::Closed:
        m_window->close();
        break;

      case sf::Event::MouseButtonPressed:
        if(event.mouseButton.button == sf::Mouse::Left)
        {
          m_gameWorld.resetSelected();
          if(!ImGui::IsAnyItemFocused() &&
             !ImGui::IsMouseHoveringAnyWindow() &&
             !ImGui::IsAnyItemHovered())
          {
            if (GameOptions::s_IsUnitMenuActive)
            {
              switch(g_iUnitType)
              {
              case RTSGame::UNIT_TYPE::kARCHER:
                m_gameWorld.createUnit(UNIT_TYPE::kARCHER, tileX, tileY);
                break;
              case RTSGame::UNIT_TYPE::kKNIGHT:
                m_gameWorld.createUnit(UNIT_TYPE::kKNIGHT, tileX, tileY);
                break;
              case RTSGame::UNIT_TYPE::kCROSSBOW:
                m_gameWorld.createUnit(UNIT_TYPE::kCROSSBOW, tileX, tileY);
                break;

              default:
                break;
              }
            }

            if (GameOptions::s_IsPlayActive)
            {
              m_startArea = true;
              m_mouseMapClick.x = tileX;
              m_mouseMapClick.y = tileY;
            }
          }

        }
        break;

      case sf::Event::MouseButtonReleased:
        if (GameOptions::s_IsPlayActive)
        {
          if (!ImGui::IsAnyItemFocused() &&
              !ImGui::IsMouseHoveringAnyWindow() &&
              !ImGui::IsAnyItemHovered())
          {
            m_startArea = false;
            m_mouseRelease.x = tileX;
            m_mouseRelease.y = tileY;
          }
        }
        break;

      default:
        break;
      }
    }

    g_time()._update();
    ge_frame_mark();
    updateFrame();
    renderFrame();
    ge_frame_clear();
  }

  postDestroy();
}

void
RTSApplication::updateFrame() {
  static sf::Clock myClock;
  sf::Time elapsedTime = myClock.getElapsedTime();

  float deltaTime = g_time().getFrameDelta();

  m_fpsTimer += deltaTime;
  if(1.0f < m_fpsTimer)
  {
    m_framesPerSecond = m_fpsCounter;
    m_fpsCounter = 0.0f;
    m_fpsTimer = 0.0f;
  }
  m_fpsCounter += 1.0f;

  //Update the interface
  ImGui::SFML::Update(*m_window, elapsedTime);

  //Begin the menu 
  mainMenu(this);

  //Check for camera movement
  Vector2 axisMovement(FORCE_INIT::kForceInitToZero);
  Vector2I mousePosition;
  mousePosition.x = sf::Mouse::getPosition(*m_window).x;
  mousePosition.y = sf::Mouse::getPosition(*m_window).y;

  auto map = m_gameWorld.getTiledMap();
  int32 tileX, tileY;
  sf::Vector2i mousePos = sf::Mouse::getPosition();
  map->getScreenToMapCoords(mousePos.x, mousePos.y, tileX, tileY);

  if(!ImGui::IsAnyItemFocused() &&
     !ImGui::IsMouseHoveringAnyWindow() &&
     !ImGui::IsAnyItemHovered())
  {
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
      //Terrain editor
      if(GameOptions::s_IsEditorActive)
      {

        for(uint32 i = 0; i < GameOptions::s_SizeOfBrush; i++)
        {
          for(uint32 j = 0; j < GameOptions::s_SizeOfBrush; j++)
          {
            map->setType(tileX + i, tileY + j, g_iTerrainSelected);
            map->setCost(tileX + i, tileY + j, TERRAIN_TYPE::Cost[g_iTerrainSelected]);
          }
        }
      }

      //Path finder Menu
      if(GameOptions::s_IsPathMenuActive)
      {

        map->setMark(tileX, tileY, g_iStartSelection);

        if(g_iStartSelection == PFMARK::START)
        {
          m_gameWorld.SetStartPos(tileX, tileY);
        }
        if(g_iStartSelection == PFMARK::END)
        {
          m_gameWorld.SetEndPos(tileX, tileY);
        }
      }

    }
  }

  if(0 == mousePosition.x ||
     sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
     sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
  {
#ifdef MAP_IS_ISOMETRIC
    axisMovement += Vector2(-1.f, 1.f);
#else
    axisMovement += Vector2(-1.f, 0.f);
#endif
  }
  if(GameOptions::s_Resolution.x - 1 == mousePosition.x ||
     sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
     sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
  {
#ifdef MAP_IS_ISOMETRIC
    axisMovement += Vector2(1.f, -1.f);
#else
    axisMovement += Vector2(1.f, 0.f);
#endif
  }
  if(0 == mousePosition.y ||
     sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
     sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
  {
#ifdef MAP_IS_ISOMETRIC
    axisMovement += Vector2(-1.f, -1.f);
#else
    axisMovement += Vector2(0.f, -1.f);
#endif
  }
  if(GameOptions::s_Resolution.y - 1 == mousePosition.y ||
     sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
     sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
  {
#ifdef MAP_IS_ISOMETRIC
    axisMovement += Vector2(1.f, 1.f);
#else
    axisMovement += Vector2(0.f, 1.f);
#endif
  }

  axisMovement *= GameOptions::s_MapMovementSpeed * deltaTime;

  m_gameWorld.getTiledMap()->moveCamera(axisMovement.x, axisMovement.y);

  //Update the world
  m_gameWorld.update(deltaTime);
}

void
RTSApplication::renderFrame() {
  m_window->clear(sf::Color::Blue);

  m_gameWorld.render();

  ImGui::SFML::Render(*m_window);

  if (GameOptions::s_IsPlayActive)
  {
    Vector2 mousePosition;
    mousePosition.x = sf::Mouse::getPosition(*m_window).x;
    mousePosition.y = sf::Mouse::getPosition(*m_window).y;
    m_cursorTexture->setPosition(mousePosition);
    m_cursorTexture->draw();

    if (m_startArea)
    {
      int32 x, y;
      m_gameWorld.getTiledMap()->
        getMapToScreenCoords(m_mouseMapClick.x, m_mouseMapClick.y, x, y);
      Vector2 v2;
      v2.x = x;
      v2.y = y;

      int32 sizeX = mousePosition.x - (x + TILESIZE_X / 2);
      int32 sizeY = mousePosition.y - (y + TILESIZE_Y / 2);

      if ((sizeX > 50 && sizeY > 50) ||
        (sizeX < -50 && sizeY < -50))
      {
        sf::RectangleShape rectangle(sf::Vector2f(sizeX, sizeY));
        rectangle.setFillColor(sf::Color::Transparent);
        rectangle.setOutlineThickness(1);

        rectangle.setPosition(x + TILESIZE_X / 2, y + TILESIZE_Y / 2);
        m_gameWorld.fillSelectedVector(v2, mousePosition);
        
        m_window->draw(rectangle);
      }
    }
  }
  m_window->display();
}

void
RTSApplication::postInit() {
  m_gameWorld.init(m_window);
  m_gameWorld.updateResolutionData();
}

void
RTSApplication::postDestroy() {
  m_gameWorld.destroy();
}



void
loadMapFromFile(RTSApplication* pApp) {
  OPENFILENAMEW ofn = { 0 };

  WString fileName;
  fileName.resize(MAX_PATH);
  bool bMustLoad = false;

  Path currentDirectory = FileSystem::getWorkingDirectoryPath();

  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = nullptr;
  ofn.lpstrDefExt = L".bmp";
  ofn.lpstrFilter = L"Bitmap File\0*.BMP\0All\0*.*\0";
  ofn.lpstrInitialDir = L"Maps\\";
  ofn.lpstrFile = &fileName[0];
  ofn.lpstrFile[0] = '\0';
  ofn.nMaxFile = MAX_PATH;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  if(GetOpenFileNameW(&ofn))
  {
    if(fileName.size() > 0)
    {
      bMustLoad = true;
    }
  }

  SetCurrentDirectoryW(UTF8::toWide(currentDirectory.toString()).c_str());

  if(bMustLoad)
  {
    pApp->getWorld()->getTiledMap()->loadFromImageFile(pApp->getRenderWindow(),
                                                       toString(fileName));
  }
}

void
mainMenu(RTSApplication* pApp) {
  if(ImGui::BeginMainMenuBar())
  {
    if(ImGui::BeginMenu("Map"))
    {
      if(ImGui::MenuItem("Load...", "CTRL+O"))
      {
        loadMapFromFile(pApp);
      }
      if(ImGui::MenuItem("Save...", "CTRL+S"))
      {

      }
      ImGui::Separator();

      if(ImGui::MenuItem("Quit", "CTRL+Q"))
      {
        pApp->getRenderWindow()->close();
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  ImGui::Begin("Game Options");
  {
    ImGui::Text("Framerate: %f", pApp->getFPS());

    ImGui::SliderFloat("Map movement speed X",
                       &GameOptions::s_MapMovementSpeed.x,
                       0.0f,
                       10240.0f);
    ImGui::SliderFloat("Map movement speed Y",
                       &GameOptions::s_MapMovementSpeed.y,
                       0.0f,
                       10240.0f);

    ImGui::Checkbox("Show grid", &GameOptions::s_MapShowGrid);

    /*ImGui::Spacing(5);*/

    ImGui::Text("Extra Windows");
    ImGui::Separator();

    if (!GameOptions::s_IsPlayActive)
    {
      if (ImGui::Checkbox("Activate Editor", &GameOptions::s_IsEditorActive))
      {
        GameOptions::s_IsPathMenuActive = false;
        GameOptions::s_IsUnitMenuActive = false;
      }

      if (ImGui::Checkbox("Activate Path Finder", &GameOptions::s_IsPathMenuActive))
      {
        GameOptions::s_IsEditorActive = false;
        GameOptions::s_IsUnitMenuActive = false;
      }

      if (ImGui::Checkbox("Active Unit Editor", &GameOptions::s_IsUnitMenuActive))
      {
        GameOptions::s_IsEditorActive = false;
        GameOptions::s_IsPathMenuActive = false;
      }
    }
  }
  ImGui::End();

  if(GameOptions::s_IsEditorActive)
  {

    // Editor
    ImGui::Begin("Editor");
    {
      ImGui::Separator();
      ImGui::Text("Terrain Texture");


      for(uint32 i = 0; i < TERRAIN_TYPE::kNumObjects; i++)
      {
        ImGui::RadioButton(TERRAIN_TYPE::ES[i].c_str(),
                           &g_iTerrainSelected,
                           static_cast<TERRAIN_TYPE::E> (i));
      }

      ImGui::SliderInt("Brush Size", &GameOptions::s_SizeOfBrush, 1, 10);

    }
    ImGui::End();
  }

  if(GameOptions::s_IsPathMenuActive)
  {

    ImGui::Begin("Path finders");
    {
      ImGui::Text("Select Positions");
      /* ImGui::Spacing(3);*/

      ImGui::RadioButton("Start Position", &g_iStartSelection, 1);
      ImGui::RadioButton("End Position", &g_iStartSelection, 10);
      /*ImGui::Spacing(4);*/

      ImGui::Separator();
      ImGui::Text("Path Finder");
      /*ImGui::Spacing(3);*/

      for(uint32 i = 0; i < TYPE_PATH_FINDER::NUMBOJ; i++)
      {
        ImGui::RadioButton(TYPE_PATH_FINDER::ES[i].c_str(),
                           &g_iPathFinders,
                           static_cast<TYPE_PATH_FINDER::E> (i));
      }

      /*ImGui::Spacing(5);*/
      if(ImGui::Button("Start", { 200, 50 }))
      {
        pApp->getWorld()->setCurrentWalker(g_iPathFinders);
        pApp->getWorld()->ResetWalker();
      }

    }
    ImGui::End();
  }

  if(GameOptions::s_IsUnitMenuActive)
  {
    GameOptions::s_IsPlayActive = false;
    // Editor
    ImGui::Begin("Editor");
    {
      for(uint32 i = 0; i < RTSGame::UNIT_TYPE::kNUMOBJ; i++)
      {
        ImGui::RadioButton(RTSGame::UNIT_TYPE::unitType[i].c_str(),
                           &g_iUnitType,
                           static_cast<UNIT_TYPE::E> (i));
      }
      
      ImGui::Spacing();
      ImGui::Spacing();

      if(ImGui::Button("Clear units", { 100, 50 }))
      {
        pApp->getWorld()->clearUnits();
      } ImGui::SameLine();

      if (ImGui::Button("Play", { 100, 50 }))
      {
        GameOptions::s_IsPlayActive = true;
        pApp->getRenderWindow()->setMouseCursorVisible(false); // Hide cursor
        g_iUnitType = -1;
        pApp->startMusic();
        pApp->setVolume(GameOptions::s_volume);
      }

    }
    ImGui::End();
  }

  if (GameOptions::s_IsPlayActive)
  {
    GameOptions::s_IsUnitMenuActive = false;
    ImGui::Begin("Play");
    {
      if (ImGui::SliderInt("Volume",
                           &GameOptions::s_volume,
                           0,
                           100))
      {
        pApp->setVolume(GameOptions::s_volume);
      }
      ImGui::Separator();
      ImGui::Spacing();
      
      for (uint32 i = 1; i < TYPE_PATH_FINDER::NUMBOJ; i++)
      {
        ImGui::RadioButton(TYPE_PATH_FINDER::ES[i].c_str(),
                           &g_iPathFinders,
                           static_cast<TYPE_PATH_FINDER::E> (i));
      }
      ImGui::Spacing;

      if (ImGui::Button("unPlay", { 100, 50 }))
      {
        GameOptions::s_IsUnitMenuActive = true;
        pApp->getRenderWindow()->setMouseCursorVisible(true); // Hide cursor
        pApp->pauseMusic();
      }
    }
    ImGui::End();
  }
}
}