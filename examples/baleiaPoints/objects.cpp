#include "objects.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Objects::initializeGL(GLuint program) {
  terminateGL();

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  m_objects.clear();
  spawnObjects();
}

Objects::Object Objects::createObject(float scale) {
  Object object;
  // TODO:Objects must come from the right to the left

  auto &re{m_randomEngine};  // Shortcut

  // Randomly choose for obstacle and food
  object.m_type = rand() & 1;

  // If food triangle
  if (object.m_type == 1) {
    object.m_polygonSides = 3;
  } else {
    std::uniform_int_distribution<int> randomSides(6, 20);
    object.m_polygonSides = randomSides(re);
  }

  // Choose a random color (actually, a grayscale)
  std::uniform_real_distribution<float> randomIntensity(0.5f, 1.0f);
  object.m_color = glm::vec4(1) * randomIntensity(re);

  object.m_color.a = 1.0f;
  object.m_rotation = 0.0f;
  object.m_scale = scale;
  //  object.m_translation = {1.5f, translation.y};

  // Choose a random angular velocity TODO: HERE
  object.m_angularVelocity = m_randomDistY(re);

  // Choose a random direction FIX HERE
  glm::vec2 direction{m_randomDistX(re), 0};
  object.m_velocity = glm::normalize(direction) / 7.0f;

  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  const auto step{M_PI * 2 / object.m_polygonSides};
  std::uniform_real_distribution<float> randomRadius(0.15f, 0.2f);
  for (const auto angle : iter::range(0.0, M_PI * 2, step)) {
    const auto radius{randomRadius(re)};
    positions.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO
  abcg::glGenBuffers(1, &object.m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, object.m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &object.m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(object.m_vao);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, object.m_vbo);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
  do {
    object.m_translation = {1.0f, m_randomDistY(m_randomEngine)};
  } while (glm::length(object.m_translation) < 0.5f);
  return object;
}

void Objects::paintGL() {
  abcg::glUseProgram(m_program);

  for (const auto &object : m_objects) {
    abcg::glBindVertexArray(object.m_vao);

    abcg::glUniform4fv(m_colorLoc, 1, &object.m_color.r);
    abcg::glUniform1f(m_scaleLoc, object.m_scale);
    abcg::glUniform1f(m_rotationLoc, object.m_rotation);

    //    for (auto i : {-3, 0, 3}) {
    //      for (auto j : {-3, 0, 3}) {
    if (object.m_translation.x > -1.0f) {
      abcg::glUniform2f(m_translationLoc, object.m_translation.x,
                        object.m_translation.y);

      abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, object.m_polygonSides + 2);
    }
    //    }

    abcg::glBindVertexArray(0);
  }

  abcg::glUseProgram(0);
}

void Objects::terminateGL() {
  for (auto object : m_objects) {
    abcg::glDeleteBuffers(1, &object.m_vbo);
    abcg::glDeleteVertexArrays(1, &object.m_vao);
  }
}

void Objects::update(const Player &player, float deltaTime) {
  if (m_spawnObjectsTimer.elapsed() > m_timer) {
    spawnObjects();
  }
  for (auto &object : m_objects) {
    //    object.m_translation -= player.m_velocity * deltaTime;
    object.m_rotation = glm::wrapAngle(object.m_rotation +
                                       object.m_angularVelocity * deltaTime);
    object.m_translation += object.m_velocity * deltaTime;
    // Wrap-around
    //    if (object.m_translation.x < -2.0f) object.m_translation.x += 3.0f;
    //    if (object.m_translation.x > +2.0f) object.m_translation.x -= 3.0f;
    //    if (object.m_translation.y < -2.0f) object.m_translation.y += 3.0f;
    //    if (object.m_translation.y > +2.0f) object.m_translation.y -= 3.0f;
  }
}
void Objects::spawnObjects() {
  m_spawnObjectsTimer.restart();
  std::uniform_real_distribution<double> timer{2.0, 3.0};

  m_timer = timer(m_randomEngine);
  m_objects.resize(m_objects.size() + 4);

  for (auto &object : m_objects) {
    if (object.m_velocity == glm::vec2(0)) {
      object = createObject(0.2);
    }

    // Make sure the asteroid won't collide with the ship
    //    do {
    //      object.m_translation = {0.8f, m_randomDistY(m_randomEngine)};
    //    } while (glm::length(object.m_translation) < 0.5f);
  }
}
