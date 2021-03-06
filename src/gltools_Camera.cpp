#include "gltools_Camera.hpp"

#include "gltools_IO.hpp"
#include "Settings.hpp"
#include "cpptools_Logger.hpp"
#include <mutex>
#include <cstdlib>

namespace imog {

// ====================================================================== //
// ====================================================================== //
// Param constructor
// ====================================================================== //

Camera::Camera(float speed, float fov)
    : m_fov(fov), m_offset(0.f), m_centeredOnTarget(false), speed(speed) {

  pivot.rot = glm::vec3(Settings::mainCameraRot, 0.0f);
}

Camera::~Camera() { this->target.reset(); }


// ====================================================================== //
// ====================================================================== //
// Set offset to zero
// ====================================================================== //
void Camera::resetOffset() { m_offset = glm::vec3{0.f}; }

// ====================================================================== //
// ====================================================================== //
// Getters for view, proj and viewproj
// ====================================================================== //

glm::mat4 Camera::view() const { return m_view; }
glm::mat4 Camera::proj() const { return m_proj; }
glm::mat4 Camera::viewproj() const { return m_viewproj; }

// ====================================================================== //
// ====================================================================== //
// Radian based zoom == fov variation
// ====================================================================== //

void Camera::zoom(float variation) {
  if (glfwGetKey(IO::window(), GLFW_KEY_Z) == GLFW_PRESS) {
    m_offset.z += variation;
  } else if (glfwGetKey(IO::window(), GLFW_KEY_X) == GLFW_PRESS) {
    m_offset.x += variation;
  } else if (glfwGetKey(IO::window(), GLFW_KEY_Y) == GLFW_PRESS) {
    m_offset.y += variation;
  } else {
    m_fov += glm::radians(variation);
  }
}

// ====================================================================== //
// ====================================================================== //
// Define what to make every frame to update the viewport
// ====================================================================== //

void Camera::frame() {
  static glm::vec3 modY;

  if (this->target) {
    // Get center of the skeleton
    if (!m_centeredOnTarget) {
      m_centeredOnTarget = true;
      modY               = pivot.up() * this->target->pos.y;
    }
    // Follow on XZ
    pivot.pos = (this->target->pos * Math::vecXZ) + modY;
  } else {
    m_centeredOnTarget = false;
  }

  auto offX = pivot.right() * m_offset.x;
  auto offY = pivot.up() * m_offset.y;
  auto offZ = pivot.front() * m_offset.z;
  pivot.pos += offX + offY + offZ;

  auto modZ = pivot.front() * Settings::mainCameraPos.z;
  auto eye  = pivot.pos - modZ;

  auto modX = (this->cinemaLike && this->target)
                  ? pivot.right() * Settings::mainCameraPos.x
                  : Math::nullVec;

  m_proj     = glm::perspective(m_fov, IO::windowAspectRatio(), m_near, m_far);
  m_view     = glm::lookAt(eye, pivot.pos + modX, Math::unitVecY);
  m_viewproj = m_proj * m_view;
}

} // namespace imog
