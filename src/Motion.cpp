#include "Motion.hpp"

#include <tuple>
#include <numeric>
#include <fstream>
#include <iostream>

#include "Logger.hpp"
#include "Loader.hpp"
#include "Strings.hpp"
#include "Settings.hpp"


namespace brave {

// * Helpers

// // ====================================================================== //
// // ====================================================================== //
// // Interpolate between two frames
// // ====================================================================== //

// std::vector<Frame> lerp(Frame F1, Frame F2, uint steps = 0u) {
//   assert(F1.rotations.size() == F2.rotations.size());
//   std::vector<Frame> lerpFrames;

//   if (steps < 1)
//     return lerpFrames;
//   else if (steps < 2)
//     steps = 2;
//   float alphaStep = 1.0f / (float)steps;

//   glm::vec3 rootRot{0.f};
//   {
//     auto fv1 = Math::rotToVec(F1.rotations[0]);
//     auto fv2 = Math::rotToVec(F2.rotations[0]);
//     rootRot  = Math::dirAngle(fv1, fv2, Math::unitVecY) * Math::unitVecY;
//   }

//   for (auto alpha = 0.1f; alpha <= 1.0f; alpha += alphaStep) {
//     Frame frame;
//     frame.translation = glm::mix(F1.translation, F2.translation, alpha);

//     frame.rotations.push_back(F1.rotations[0] + alpha * rootRot);
//     for (auto i = 1u; i < F1.rotations.size(); ++i) {
//       auto newRot = glm::mix(F1.rotations[i], F2.rotations[i], alpha);
//       frame.rotations.push_back(newRot);
//     }
//     lerpFrames.push_back(frame);
//   }

//   return lerpFrames;
// }



// * Frame methods

// ====================================================================== //
// ====================================================================== //
// Sum rotations of frame
// ====================================================================== //

glm::vec3 Frame::value() const {
  glm::vec3 wJR{1.f};
  glm::vec3 wRT{0.f}; // !
  glm::vec3 wRR{0.f}; // !

  glm::vec3 JR{0.f};
  glm::vec3 RT = this->translation;
  glm::vec3 RR = this->rotations.at(0);
  for (const auto& rot : this->rotations) JR += rot;

  return (wRT * RT) + (wRR * RR) + (wJR * (JR - RR));
}

// ====================================================================== //
// ====================================================================== //
// Lerp frame A to frame B at alpha point
// ====================================================================== //

Frame Frame::lerpOne(const Frame& f2, float alpha) const {
  auto tf1 = this->translation;
  auto rf1 = this->rotations;
  auto tf2 = f2.translation;
  auto rf2 = f2.rotations;

  Frame f;
  f.translation = glm::mix(tf1, tf2, alpha);
  try {
    for (auto i = 0u; i < rf1.size(); ++i) {
      f.rotations.push_back(glm::mix(rf1[i], rf2[i], alpha));
    }
  } catch (std::exception&) {
    LOGE("F1 and F2 doesn't have the same number of Joints?")
  }
  return f;
}

// ====================================================================== //
// ====================================================================== //
// Lerp from frame A to frame B
// ====================================================================== //

std::vector<Frame> Frame::lerpTransition(const Frame& f2, uint steps) const {
  std::vector<Frame> lerpFrames;
  if (steps < 1u) return lerpFrames;
  float alphaStep = 1.0f / glm::clamp((float)steps, 2.f, 100.f);

  for (auto alpha = 0.1f; alpha <= 1.0f; alpha += alphaStep) {
    auto frame = this->lerpOne(f2, alpha);
    lerpFrames.push_back(frame);
  }
  return lerpFrames;
}



// * Motion methods

// ====================================================================== //
// ====================================================================== //
// Constructor
// ====================================================================== //

std::shared_ptr<Motion> Motion::create(const std::string& name,
                                       const std::string& filepath,
                                       loopMode           lm,
                                       uint               steps) {
  auto m  = loader::BVH(filepath);
  m->name = name;

  // === BEFORE CLEAN ===
  m->m_maxStep = 0u;

  for (auto i = 0u; i < m->frames.size(); ++i) {
    auto f = m->frames.at(i);

    // Get Max distance
    if (i < m->frames.size() - 2u) {
      auto f2   = m->frames.at(i + 1u);
      auto step = glm::distance(f2.translation, f.translation);
      if (step > m->m_maxStep) m->m_maxStep = step;
    }
  }


  // === CLEAN ===
  //
  m->clean(lm, steps);


  // === AFTER CLEAN ===

  //
  float _inf = std::numeric_limits<float>::max();
  float minRX, minRY, minRZ, minTX, minTY, minTZ;
  minRX = minRY = minRZ = minTX = minTY = minTZ = _inf;
  //

  // Get data
  for (const auto& f : m->frames) {
    // T
    if (f.translation.x < minTX) minTX = f.translation.x;
    if (f.translation.y < minTY) minTY = f.translation.y;
    if (f.translation.z < minTZ) minTZ = f.translation.z;
    // R
    if (f.rotations.at(0).x < minRX) minRX = f.rotations.at(0).x;
    if (f.rotations.at(0).y < minRY) minRY = f.rotations.at(0).y;
    if (f.rotations.at(0).z < minRZ) minRZ = f.rotations.at(0).z;
  }

  // Set data
  for (auto& f : m->frames) {
    // T
    f.translation.x -= minTX;
    f.translation.y -= minTY;
    f.translation.z -= minTZ;
    // R set between [0,360]
    f.rotations.at(0) = glm::mod(f.rotations.at(0), 360.f);
    if (f.rotations.at(0).x < 0.f) f.rotations.at(0).x += 360.f;
    if (f.rotations.at(0).y < 0.f) f.rotations.at(0).y += 360.f;
    if (f.rotations.at(0).z < 0.f) f.rotations.at(0).z += 360.f;
    // R back to 0
    f.rotations.at(0).x -= minRX;
    f.rotations.at(0).y -= minRY;
    f.rotations.at(0).z -= minRZ;
  }

  return m;
}

// ====================================================================== //
// ====================================================================== //
// Minor methods
// ====================================================================== //

float Motion::maxStep() const { return m_maxStep; }

bool Motion::isMix() { return Strings::contains(this->name, "_"); }

Frame Motion::linkedFrame(uint frameIdx, float alpha) const {
  Frame f;
  if (!this->linked) return f;
  auto factor = (float)linked->frames.size() / (float)frames.size();
  // Get errors with 'ceil' (get out-of-range values). Using 'floor' instead.
  auto cf = floorf(frameIdx * factor);
  return frames.at(cf).lerpOne(linked->frames.at(cf), alpha);
}

// ====================================================================== //
// ====================================================================== //
// Clean any motion to get a smoother loop
// ====================================================================== //

void Motion::clean(loopMode lm, uint steps) {
  this->frames.erase(this->frames.begin());
  if (lm == loopMode::none) return;

  // Get low errror frames of animation to generate a loop
  uint B, E;
  {
    uint  nFrames = frames.size();
    uint  limitA  = nFrames / 2.f;
    uint  limitB  = nFrames - limitA;
    float auxDiff = std::numeric_limits<float>::max();

    for (auto f1 = 0u; f1 < limitA; ++f1) {
      glm::vec3 f1Val = this->frames.at(f1).value();

      for (auto f2 = limitB; f2 < nFrames; ++f2) {
        glm::vec3 f2Val = this->frames.at(f2).value();

        auto diff = glm::compAdd(glm::abs(f1Val - f2Val));
        if (diff < auxDiff) { std::tie(auxDiff, B, E) = {diff, f1, f2}; }
      }
    }
  }

  // Clean
  if (lm == loopMode::shortLoop) {
    std::vector<Frame> auxFrames;
    auxFrames.reserve(E);
    for (auto f = B; f < E; ++f) { auxFrames.push_back(this->frames.at(f)); }
    this->frames = auxFrames; // Store
  }

  // Lerp
  auto first = this->frames.front();
  auto last  = this->frames.back();
  for (const auto& nF : last.lerpTransition(first, steps)) {
    this->frames.push_back(nF);
  }
}

// ====================================================================== //
// ====================================================================== //
// Mix any motion with other and get a new animation
// that conect both smoothly
// ====================================================================== //

Motion::mixMap Motion::mix(const std::shared_ptr<Motion>& m2) {
  auto createTransitionMotion = [&](uint idxF1, uint idxF2) {
    auto mo      = std::make_shared<Motion>();
    mo->joints   = this->joints;
    mo->timeStep = (this->timeStep + m2->timeStep) * 0.5f;

    auto f1 = this->frames.at(idxF1);
    auto f2 = m2->frames.at(idxF2);
    for (const auto& nF : f1.lerpTransition(f2, 10u)) {
      mo->frames.push_back(nF);
    }

    return mo;
  };

  //---

  // for heat map visualization
  auto _folder = "./assets/plotdata/";
  mkdir(_folder);
  auto          _prefix = _folder + this->name + "_" + m2->name;
  std::ofstream heatmap(_prefix + "__heatmap.txt");
  std::ofstream refFrames(_prefix + "__refFrames.txt");

  //---

  mixMap mm;
  {

    uint  auxF2   = 0;
    float auxDiff = std::numeric_limits<float>::infinity();

    for (auto f1 = 0u; f1 < this->frames.size(); f1++) {
      auxF2             = 0;
      auxDiff           = std::numeric_limits<float>::infinity();
      glm::vec3 f1Value = this->frames.at(f1).value();

      for (auto f2 = 0u; f2 < m2->frames.size(); f2++) {
        glm::vec3 f2Value = m2->frames.at(f2).value();

        float diff = glm::compAdd(glm::abs(f1Value - f2Value));
        if (diff < auxDiff) { std::tie(auxDiff, auxF2) = {diff, f2}; }

        // Write to heatmap
        (f2 < m2->frames.size() - 1) ? heatmap << diff << " "
                                     : heatmap << diff << "\n";
      }
      // Write to ref frames. For mark winner frames on heatmap
      refFrames << f1 << " " << auxF2 << "\n";

      // Insert winner frames and its transition on the map
      auto tMo = createTransitionMotion(f1, auxF2);
      mm.insert({f1, {auxF2, tMo}});
    }
  }

  return mm;
}

} // namespace brave
