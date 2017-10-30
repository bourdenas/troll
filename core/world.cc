#include "world.h"

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <range/v3/view/transform.hpp>

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

void World::Init() {
  LoadScene("main.scene");
  LoadSprites();
}

void World::Run() {}

void World::LoadScene(const std::string& scene) {
  const auto scene_pb = LoadTextProto<ScenePb>("../data/scenes/" + scene);
  scene_.reset(new Scene(scene_pb.id()));
}

void World::UnloadScene() {
  scene_.reset(nullptr);
  objects_.clear();
  characters_.clear();
}

void World::LoadSprites() {
  const auto& sprites = LoadTextProtoFromPath<Sprite>("../data/sprites/");

  objects_ = sprites | ranges::view::transform([](const Sprite& sprite) {
               return std::make_pair(
                   sprite.id(),
                   std::unique_ptr<Object>(new Object(sprite.id(), sprite)));
             });
  std::cout << "sprites loaded" << std::endl;
}

}  // namespace troll
