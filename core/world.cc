#include "world.h"

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

#include "sprite.pb.h"

namespace troll {

void World::Init() {
  LoadSprites();
}

void World::Run() {}

void World::LoadScene(const std::string& scene) {}

void World::UnloadScene() {}

void World::LoadSprites() {
  const boost::filesystem::path sprite_path("../data/sprites/");
  for (auto it = boost::filesystem::directory_iterator(sprite_path);
       it != boost::filesystem::directory_iterator(); ++it) {
    std::cout << "Loading sprite file '" << it->path() << "'..." << std::endl;
    Sprite sprite;
    std::fstream istream(it->path().string(), std::ios::in);
    google::protobuf::io::IstreamInputStream pbstream(&istream);
    google::protobuf::TextFormat::Parse(&pbstream, &sprite);
    std::cout << "sprite '" << sprite.id() << "' loaded." << std::endl;

    objects_[sprite.id()] =
        std::unique_ptr<Object>(new Object(sprite.id(), sprite));
  }
}

}  // namespace troll
