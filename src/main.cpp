#include "IO.hpp"
#include "Math.hpp"
#include "Settings.hpp"
#include "Skeleton.hpp"
#include "Renderable.hpp"
#include "helpers/Consts.hpp"
#include "helpers/Colors.hpp"
using namespace brave;


// * ===================================================================== * //
void DBG_VEC(std::shared_ptr<brave::Camera> camera,
             glm::vec3                      vec,
             glm::vec3                      color = glm::vec3(1, 0, 0)) {

  auto DBG = Renderable::create(false, "DBG", Figures::cube, "", Colors::red);
  DBG->transform.scl = glm::vec3(0.25f);
  DBG->color(color);
  for (auto i = -25; i < 25; i++) {
    DBG->transform.pos = vec * (0.1f * i);
    DBG->draw(camera);
  }
};
// * ===================================================================== * //


int main(int argc, char const* argv[]) {

  // ---------------------------------------------------------
  // --- Initialization --------------------------------------

  Settings::init(Paths::settings);

  auto camera = std::make_shared<Camera>(Settings::mainCameraSpeed);

  IO::windowInit(camera);

  auto light = std::make_shared<Light>(Settings::mainLightPos,
                                       Settings::mainLightColor,
                                       Settings::mainLightIntensity);

  // ------------------------------------ / Initialization ---
  // ---------------------------------------------------------


  // ---------------------------------------------------------
  // --- Skeleton --------------------------------------------

  auto skeleton = Skeleton(camera, 0.5f);
  skeleton.addMotion("Idle", Motions::idle, loopMode::loop, 25u);
  // skeleton.addMotion("Run", Motions::run, loopMode::shortLoop);
  skeleton.animate();

  skeleton.onKey(GLFW_KEY_0, [&]() { skeleton.play = !skeleton.play; });

  IO::keyboardAddAction(GLFW_KEY_0, IO::kbState::repeat, [&]() {
    skeleton.play = !skeleton.play;
  });

  skeleton.onKey(GLFW_KEY_1, [&]() {
    skeleton.setMotion("Run");
    skeleton.moveF(true);
  });
  skeleton.onKey(GLFW_KEY_2, [&]() {
    skeleton.setMotion("Idle");
    skeleton.moveF(false);
  });

  // ------------------------------------------ / Skeleton ---
  // ---------------------------------------------------------


  // ---------------------------------------------------------
  // --- Loop ------------------------------------------------

  auto updateFn = [&]() {
    camera->speed(Settings::mainCameraSpeed);
    light->pos(Settings::mainLightPos);
    light->color(Settings::mainLightColor);
    light->intensity(Settings::mainLightIntensity);
  };

  auto renderFn = [&]() {
    skeleton.draw();
    camera->frame();
    Shader::poolUpdate(camera, light);
    Renderable::poolDraw(camera);
  };

  IO::windowLoop(renderFn, updateFn);

  // Skeleton invoke a thread that use render, shader and texture pools.
  // So if we don't remove skeleton object before remove pools' content and
  // let that OS' memory manager remove unfree data. May ocur
  delete &skeleton;

  // ---------------------------------------------- / Loop ---
  // ---------------------------------------------------------

  return 0;
}
