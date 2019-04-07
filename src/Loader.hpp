#pragma once
#include <vector>
#include "Math.hpp"

// Stuff with data types used for loaders
#include "Motion.hpp"
#include "Renderable.hpp"

namespace brave {
namespace loader {

  // Return a struct with data needed to rendering from a OBJ file
  Renderable::data OBJ(const std::string& filePath);

  // Return data needed to create a Skeleton from a BVH file
  std::shared_ptr<Motion> BVH(const std::string& bvhFilePath, bool makeLoop);

} // namespace loader
} // namespace brave
