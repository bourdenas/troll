#include "core/troll-core.h"

#include <fstream>
#include <iostream>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <boost/filesystem.hpp>
#include <range/v3/view/transform.hpp>
#include "sdl/renderer.h"

#include "proto/scene.pb.h"
#include "proto/sprite.pb.h"

namespace troll {

namespace {
// Load a proto message from text file.
template <class Message>
Message LoadTextProto(const std::string& uri) {
  std::cout << "Loading proto text file '" << uri << "'..." << std::endl;
  std::fstream istream(uri, std::ios::in);
  google::protobuf::io::IstreamInputStream pbstream(&istream);

  Message message;
  google::protobuf::TextFormat::Parse(&pbstream, &message);
  std::cout << "Proto message '" << message.id() << "' loaded." << std::endl;
  return message;
}

// Load proto message from all text files under a path.
template <class Message>
std::vector<Message> LoadTextProtoFromPath(const std::string& path) {
  const boost::filesystem::path resource_path(path);
  std::vector<Message> messages;
  for (const auto& p : boost::filesystem::directory_iterator(resource_path)) {
    messages.push_back(LoadTextProto<Message>(p.path().string()));
  }
  return messages;
}
}  // namespace

void Core::Init() {
  renderer_ = std::make_unique<Renderer>();
  renderer_->Init(800, 600);

  LoadScene("main.scene");
  LoadSprites();
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
    RenderFrame();
    FrameEnded(curr_time - prev_time);

    prev_time = curr_time;
  }
}

bool Core::InputHandling() {
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
  // AnimatorManager::Instance().Progress(timeSinceLastFrame);
  // Physics::Instance().Progress(timeSinceLastFrame);
  // CollisionChecker::Instance().PerformCollisions();
}

void Core::RenderFrame() {
  std::cout << "RenderFrame()" << std::endl;
  const auto texture = renderer_->LoadTexture("dkarcade.png");
  renderer_->ClearScreen();
  renderer_->BlitTexture(*texture, Box(), Box());
  std::cout << "BlitTexture()" << std::endl;
  renderer_->Flip();
}

void Core::FrameEnded(int time_since_last_frame) {
  //-- Destroy LatelyDestroyable objects
  // DestructionManager::Commit();

  ++fps_counter_.fp_count;
  fps_counter_.elapsed_time += time_since_last_frame;

  if (fps_counter_.elapsed_time < 1000) {
    return;
  }
  fps_counter_.fps = fps_counter_.fp_count;
  fps_counter_.elapsed_time = fps_counter_.fp_count = 0;
}

void Core::LoadScene(const std::string& scene_id) {
  const auto scene_pb = LoadTextProto<ScenePb>("../data/scenes/" + scene_id);

  scene_manager_ = std::make_unique<SceneManager>();
  scene_manager_->SetupScene(std::make_unique<Scene>(scene_pb.id(), scene_pb));
}

void Core::UnloadScene() {
  scene_manager_->UnloadScene();

  textures_.clear();
  characters_.clear();
  objects_.clear();
}

void Core::LoadSprites() {
  const auto& sprites = LoadTextProtoFromPath<Sprite>("../data/sprites/");

  textures_ =
      sprites | ranges::view::transform([this](const Sprite& sprite) {
        return std::make_pair(sprite.resource(),
                              renderer_->LoadTexture(sprite.resource()));
      });

  objects_ = sprites | ranges::view::transform([](const Sprite& sprite) {
               return std::make_pair(
                   sprite.id(), std::make_unique<Object>(sprite.id(), sprite));
             });
  std::cout << "sprites loaded" << std::endl;
}

}  // namespace troll
