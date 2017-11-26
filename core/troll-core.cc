#include "core/troll-core.h"

#include <fstream>

#include <absl/strings/str_cat.h>
#include <glog/logging.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <boost/filesystem.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/action/sort.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/unique.hpp>

#include "animation/animator-manager.h"
#include "core/action-manager.h"
#include "proto/animation.pb.h"
#include "proto/scene.pb.h"
#include "proto/sprite.pb.h"
#include "sdl/renderer.h"

namespace troll {

namespace {
// Load a proto message from text file.
template <class Message>
Message LoadTextProto(const std::string& uri) {
  LOG(INFO) << "Loading proto text file '" << uri << "'...";
  std::fstream istream(uri, std::ios::in);
  google::protobuf::io::IstreamInputStream pbstream(&istream);

  Message message;
  google::protobuf::TextFormat::Parse(&pbstream, &message);
  return message;
}

// Load proto message from all text files under a path.
template <class Message>
std::vector<Message> LoadTextProtoFromPath(const std::string& path,
                                           const std::string& extension) {
  const boost::filesystem::path resource_path(path);
  const std::vector<Message> messages =
      boost::filesystem::directory_iterator(resource_path) |
      ranges::view::remove_if([&extension](const auto& p) {
        return p.path().extension() != extension;
      }) |
      ranges::view::transform([](const auto& p) {
        return LoadTextProto<Message>(p.path().string());
      });
  return messages;
}

constexpr char* kDefaultFont = "fonts/times.ttf";
constexpr char* kFpsTexture = "__fps_texture";
constexpr char* kFpsSprite = "__fps_sprite";

}  // namespace

void Core::Init() {
  ActionManager::Instance().Init();

  renderer_ = std::make_unique<Renderer>();
  renderer_->Init(800, 600);

  fonts_[kDefaultFont] = renderer_->LoadFont(kDefaultFont, 16);

  textures_[kFpsTexture] = PrintFpsCounter(0);
  Sprite fps_sprite;
  fps_sprite.set_id(kFpsSprite);
  fps_sprite.set_resource(kFpsTexture);
  *fps_sprite.add_film() = textures_[kFpsTexture]->GetBoundingBox();
  sprites_[kFpsSprite] = fps_sprite;

  LoadScene("main.scene");
}

void Core::CleanUp() {
  UnloadScene();

  renderer_->CleanUp();
}

void Core::Run() {
  int curr_time = SDL_GetTicks();
  int prev_time = curr_time;

  while (InputHandling()) {
    curr_time = SDL_GetTicks();

    FrameStarted(curr_time - prev_time);
    scene_manager_->Render();
    FrameEnded(curr_time - prev_time);

    prev_time = curr_time;
  }
}

void Core::Halt() { halt_ = true; }

void Core::LoadScene(const std::string& scene_id) {
  LoadSprites();
  scene_manager_ = std::make_unique<SceneManager>(
      LoadTextProto<Scene>("../data/scenes/" + scene_id), *renderer_);
  scene_manager_->SetupScene();
}

void Core::UnloadScene() {
  textures_.clear();
  sprites_.clear();
}

bool Core::InputHandling() {
  if (halt_) {
    return false;
  }

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      return false;
    }
    if (event.type == SDL_KEYDOWN) {
      return false;
    }
  }
  return true;
}

void Core::FrameStarted(int time_since_last_frame) {
  AnimatorManager::Instance().Progress(time_since_last_frame);
  // Physics::Instance().Progress(time_since_last_frame);
  // CollisionChecker::Instance().PerformCollisions();
}

void Core::FrameEnded(int time_since_last_frame) {
  ++fps_counter_.fp_count;
  fps_counter_.elapsed_time += time_since_last_frame;

  if (fps_counter_.elapsed_time < 1000) {
    return;
  }
  fps_counter_.fps = fps_counter_.fp_count;
  fps_counter_.elapsed_time = fps_counter_.fp_count = 0;

  textures_[kFpsTexture] = PrintFpsCounter(fps_counter_.fps);
  *sprites_[kFpsSprite].mutable_film(0) =
      textures_[kFpsTexture]->GetBoundingBox();
  scene_manager_->Dirty(scene_manager_->GetSceneNodeById("fps_counter"));
}

std::unique_ptr<Texture> Core::PrintFpsCounter(int fps) {
  RGBa gray;
  gray.set_red(200);
  gray.set_green(200);
  gray.set_blue(200);
  return renderer_->CreateText(absl::StrCat(fps), *fonts_[kDefaultFont], gray,
                               RGBa());
}

void Core::LoadSprites() {
  const auto sprites =
      LoadTextProtoFromPath<Sprite>("../data/sprites/", ".sprite");
  const auto animations =
      LoadTextProtoFromPath<SpriteAnimation>("../data/sprites/", ".animation");
  AnimatorManager::Instance().Init(animations);

  ranges::action::insert(
      textures_,
      sprites | ranges::view::transform(
                    [](const Sprite& sprite) { return sprite.resource(); }) |
          ranges::action::sort | ranges::view::unique |
          ranges::view::transform([this](const std::string& resource) {
            return std::make_pair(resource, renderer_->LoadTexture(resource));
          }));

  ranges::action::insert(
      sprites_, sprites | ranges::view::transform([](const Sprite& sprite) {
                  return std::make_pair(sprite.id(), sprite);
                }));
}

}  // namespace troll
