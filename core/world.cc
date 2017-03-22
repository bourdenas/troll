#include "world.h"

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <range/v3/all.hpp>

#include "scene.pb.h"
#include "sprite.pb.h"

namespace troll {

namespace {
template <class Resource>
Resource LoadResource(const std::string& uri) {
  std::cout << "Loading resource file '" << uri << "'..." << std::endl;
  std::fstream istream(uri, std::ios::in);
  google::protobuf::io::IstreamInputStream pbstream(&istream);

  Resource resource;
  google::protobuf::TextFormat::Parse(&pbstream, &resource);
  std::cout << "Resource '" << resource.id() << "' loaded." << std::endl;
  return resource;
}

template <class Resource>
std::vector<Resource> LoadResources(const std::string& path) {
  const boost::filesystem::path sprite_path(path);
  std::vector<Resource> resources;
  for (const auto& p : boost::filesystem::directory_iterator(sprite_path)) {
    resources.push_back(LoadResource<Resource>(p.path().string()));
  }
  return resources;
}
}  // namespace

void World::Init() {
  LoadScene("main.scene");
  LoadSprites();
}

void World::Run() {}

void World::LoadScene(const std::string& scene) {
  const auto scene_pb = LoadResource<ScenePb>("../data/scenes/" + scene);
  scene_.reset(new Scene(scene_pb.id()));
}

void World::UnloadScene() {
  scene_.reset(nullptr);
  objects_.clear();
  characters_.clear();
}

void World::LoadSprites() {
  const auto& sprites = LoadResources<Sprite>("../data/sprites/");

  objects_ = sprites | ranges::view::transform([](const Sprite& sprite) {
               return std::make_pair(
                   sprite.id(),
                   std::unique_ptr<Object>(new Object(sprite.id(), sprite)));
             });
  std::cout << "sprites loaded" << std::endl;
}

}  // namespace troll
