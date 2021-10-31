#include "player.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Player::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  m_velocity = glm::vec2(0);

  std::array<glm::vec2, 4> positions{
      // Ship body
      glm::vec2{-09.5f, +12.5f},
      glm::vec2{-09.5f, -07.5f},
      glm::vec2{+09.5f, -07.5f},
      glm::vec2{+09.5f, +12.5f},

  };

  // Normalize
  for (auto &position : positions) {
    position /= glm::vec2{15.5f, 15.5f};
  }

  const std::array indices{0, 1, 3, 1, 2, 3};

  // Generate VBO
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_ebo);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Player::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_vao);

  abcg::glUniform1f(m_scaleLoc, m_scale);
  abcg::glUniform1f(m_rotationLoc, m_rotation);
  abcg::glUniform2fv(m_translationLoc, 1, &m_translation.x);
  /*
    // Restart thruster blink timer every 100 ms
    if (m_trailBlinkTimer.elapsed() > 100.0 / 1000.0)
    m_trailBlinkTimer.restart();

    if (gameData.m_input[static_cast<size_t>(Input::Up)]) {
      // Show thruster trail during 50 ms
      if (m_trailBlinkTimer.elapsed() < 50.0 / 1000.0) {
        abcg::glEnable(GL_BLEND);
        abcg::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 50% transparent
        abcg::glUniform4f(m_colorLoc, 1, 1, 1, 0.5f);

        abcg::glDrawElements(GL_TRIANGLES, 14 * 3, GL_UNSIGNED_INT, nullptr);

        abcg::glDisable(GL_BLEND);
      }
    }
  */
  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  abcg::glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Player::terminateGL() {
  abcg::glDeleteBuffers(1, &m_vbo);
  abcg::glDeleteBuffers(1, &m_ebo);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

// TODO:
void Player::update(const GameData &gameData, float deltaTime) {
  // Rotate
  //  if (gameData.m_input[static_cast<size_t>(Input::Left)])
  //    m_rotation = glm::wrapAngle(m_rotation + 4.0f * deltaTime);
  //  if (gameData.m_input[static_cast<size_t>(Input::Right)])
  //    m_rotation = glm::wrapAngle(m_rotation - 4.0f * deltaTime);

  // Apply thrust
  if (gameData.m_state == State::Playing) {
    if (gameData.m_input[static_cast<size_t>(Input::Up)] &&
        m_translation.y < 0.9f) {
      m_translation = {m_translation.x, m_translation.y + 0.6f * deltaTime};
    }
    if (gameData.m_input[static_cast<size_t>(Input::Left)] &&
        m_translation.x > -0.9f) {
      m_translation = {m_translation.x - 0.6f * deltaTime, m_translation.y};
    }
    if (gameData.m_input[static_cast<size_t>(Input::Right)] &&
        m_translation.x < 0.9f) {
      m_translation = {m_translation.x + 0.6f * deltaTime, m_translation.y};
    }
    if (gameData.m_input[static_cast<size_t>(Input::Down)] &&
        m_translation.y > -0.9f) {
      m_translation = {m_translation.x, m_translation.y - 0.6f * deltaTime};
    }
  }
}

void Player::addPoint() { points += 1; }

void Player::resetPoints() { points = 0; }

void Player::reduceLife() { life -= 1; }

void Player::resetLife() { life = 3; }
