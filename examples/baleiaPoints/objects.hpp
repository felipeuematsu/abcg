#ifndef OBJECTS_HPP_
#define OBJECTS_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "player.hpp"

class OpenGLWindow;

class Objects {
 public:
  void initializeGL(GLuint program, int quantity);
  void paintGL();
  void terminateGL();

  void update(const Player &player, float deltaTime);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_rotationLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  struct Object {
    GLuint m_vao{};
    GLuint m_vbo{};

    float m_angularVelocity{};
    glm::vec4 m_color{1};
    bool m_hit{false};
    int m_type{0};  // 0-obstacle,1-food
    int m_polygonSides{};
    float m_rotation{};
    float m_scale{};
    glm::vec2 m_translation{glm::vec2(0)};
    glm::vec2 m_velocity{glm::vec2(0)};
  };

  std::list<Object> m_objects;
  abcg::ElapsedTimer m_spawnObjectsTimer;

  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDistX{-1.0f, 0.0f};
  std::uniform_real_distribution<float> m_randomDistY{-1.0f, 1.0f};
  double m_timer{2.0};

  Objects::Object createObject(float scale);
  void spawnObjects();
};

#endif
