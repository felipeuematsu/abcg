#include "openglwindow.hpp"

#include <imgui.h>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.set(static_cast<size_t>(Input::Fire));
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.set(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.set(static_cast<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.set(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.set(static_cast<size_t>(Input::Right));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Fire));
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Right));
  }

  // Mouse events
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.set(static_cast<size_t>(Input::Fire));
    if (event.button.button == SDL_BUTTON_RIGHT)
      m_gameData.m_input.set(static_cast<size_t>(Input::Up));
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Fire));
    if (event.button.button == SDL_BUTTON_RIGHT)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Up));
  }
  /*
  if (event.type == SDL_MOUSEMOTION) {
    glm::ivec2 mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

    glm::vec2 direction{glm::vec2{mousePosition.x - m_viewportWidth / 2,
                                  mousePosition.y - m_viewportHeight / 2}};
    direction.y = -direction.y;
    m_ship.setRotation(std::atan2(direction.y, direction.x) - M_PI_2);
  }
  */
}

void OpenGLWindow::initializeGL() {
  // Load a new font
  ImGuiIO &io{ImGui::GetIO()};
  auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  m_scoreFont = io.Fonts->AddFontFromFileTTF(filename.c_str(), 32.0f);
  if (m_font == nullptr || m_scoreFont == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  // Create program to render the other objects
  m_objectsProgram = createProgramFromFile(getAssetsPath() + "objects.vert",
                                           getAssetsPath() + "objects.frag");

  abcg::glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  restart();
}

void OpenGLWindow::restart() {
  m_gameData.m_state = State::Menu;

  m_player.initializeGL(m_objectsProgram);
  m_player.resetLife();
  m_player.resetPoints();
  m_objects.initializeGL(m_objectsProgram);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if ((m_gameData.m_state == State::Win ||
       m_gameData.m_state == State::GameOver) &&
      m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }
  if (m_gameData.m_state == State::Playing) {
    checkCollisions();
    checkWinCondition();
  }

  m_player.update(m_gameData, deltaTime);
  m_objects.update(m_player, deltaTime);
}

void OpenGLWindow::paintGL() {
  update();

  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_player.paintGL(m_gameData);
  m_objects.paintGL();
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  if (m_gameData.m_state == State::Playing || m_gameData.m_state == State::GameOver) {
    ImGui::Begin("Score");
    ImGui::SetWindowPos(ImVec2(20.0f, 20.0f));

    ImGui::PushFont(m_scoreFont);
    ImGui::Text("Lives: %d", m_player.life);
    ImGui::Text("Points: %d", m_player.points);
    ImGui::PopFont();
    ImGui::End();
  }

  if (m_gameData.m_state == State::Menu) {
    ImGui::Begin("Menu");
    int buttonSize = 180;
    if (ImGui::Button("Start", ImVec2(buttonSize, 50))) {
      restart();
      m_gameData.m_state = State::Playing;
    }
    if (ImGui::Button("Quit", ImVec2(buttonSize, 50))) {
      ImGui::End();
    }
    float width = 200;
    float height = 200;
    ImGui::SetWindowPos(ImVec2((m_viewportWidth - width) / 2.0f,
                               (m_viewportHeight - ImGui::GetWindowHeight()) / 2.0f));
    ImGui::SetWindowSize(ImVec2(width, height));
    ImGui::TextWrapped("Catch the purple triangles and avoid the green objects!");
    ImGui::End();
  }
  const auto size{ImVec2(300, 160)};
  const auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                             (m_viewportHeight - size.y) / 2.0f)};
  ImGui::SetNextWindowPos(position);
  ImGui::SetNextWindowSize(size);
  ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                         ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_NoInputs};
  ImGui::Begin(" ", nullptr, flags);
  ImGui::PushFont(m_font);

  if (m_gameData.m_state == State::GameOver) {
    ImGui::Text("Game Over!");
  } else if (m_gameData.m_state == State::Win) {
    ImGui::Text("*You Win!*");
  }

  ImGui::PopFont();
  ImGui::End();
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_objectsProgram);

  m_player.terminateGL();
  m_objects.terminateGL();
}

void OpenGLWindow::checkCollisions() {
  // Check collision between Player and Obstacle or food
  for (auto &object : m_objects.m_objects) {
    const auto objectTranslation{object.m_translation};
    const auto distance{
        glm::distance(m_player.m_translation, objectTranslation)};

    if (distance < m_player.m_scale * 0.9f + object.m_scale * 0.12f) {
      object.m_hit = true;
      if (object.m_type != 1) {
        object.m_hit = true;
        m_player.reduceLife();
        if (m_player.life <= 0) {
          m_gameData.m_state = State::GameOver;
          m_restartWaitTimer.restart();
        }
      } else {
        m_player.addPoint();
      }
    }
  }
  m_objects.m_objects.remove_if([](const Objects::Object &obj) {
    return obj.m_hit || obj.m_translation.x < -1.0f;
  });
}

// Win if the player collects +10 food
void OpenGLWindow::checkWinCondition() {
  if (m_player.points >= 10) {
    m_gameData.m_state = State::Win;
    m_restartWaitTimer.restart();
  }
}
