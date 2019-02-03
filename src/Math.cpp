#include "Math.hpp"


namespace BRAVE {

glm::vec3 Math::unitVecX{1.f, 0.f, 0.f};
glm::vec3 Math::unitVecY{0.f, 1.f, 0.f};
glm::vec3 Math::unitVecZ{0.f, 0.f, 1.f};

// Wrap mat4 translation
void Math::translate(glm::mat4& mat, const glm::vec3& T) {
  mat = glm::translate(mat, T);
}
void Math::translate(glm::mat4& mat, float x, float y, float z) {
  translate(mat, glm::vec3{x, y, z});
}

// Wrap mat4 rotation
void Math::rotate(glm::mat4& mat, const glm::vec3& R) {
  mat = glm::rotate(mat, glm::radians(R.z), unitVecZ);
  mat = glm::rotate(mat, glm::radians(R.y), unitVecY);
  mat = glm::rotate(mat, glm::radians(R.x), unitVecX);
}
void Math::rotate(glm::mat4& mat, float x, float y, float z) {
  rotate(mat, glm::vec3{x, y, z});
}

// Wrap mat4 scale
void Math::scale(glm::mat4& mat, const glm::vec3& S) {
  mat = glm::scale(mat, S);
}
void Math::scale(glm::mat4& mat, float x, float y, float z) {
  scale(mat, glm::vec3{x, y, z});
}

// Numeric string vector of size 3(-1) to glm::vec3
glm::vec2 Math::glmVec2FromStr(const std::vector<std::string>& strVec) {
  float x = std::stof(strVec.at(1));
  float y = std::stof(strVec.at(2));
  return glm::vec2{x, y};
}
// Numeric string vector of size 4(-1) to glm::vec3
glm::vec3 Math::glmVec3FromStr(const std::vector<std::string>& strVec) {
  float x = std::stof(strVec.at(1));
  float y = std::stof(strVec.at(2));
  float z = std::stof(strVec.at(3));
  return glm::vec3{x, y, z};
}
// Numeric string vector of size 5(-1) to glm::vec3
glm::vec4 Math::glmVec4FromStr(const std::vector<std::string>& strVec) {
  float x = std::stof(strVec.at(1));
  float y = std::stof(strVec.at(2));
  float z = std::stof(strVec.at(3));
  float w = std::stof(strVec.at(4));
  return glm::vec4{x, y, z, w};
}

} // namespace BRAVE