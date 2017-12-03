#include "core/resource-manager.h"

#include <fstream>

#include <glog/logging.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <boost/filesystem.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/action/sort.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/map.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/unique.hpp>

#include "proto/animation.pb.h"
#include "proto/scene.pb.h"
#include "proto/sprite.pb.h"
#include "sdl/renderer.h"

namespace troll {

namespace {
// Load a proto message from text file.
template <class Message>
Message LoadTextProto(const std::string& uri) {
  DLOG(INFO) << "Loading proto text file '" << uri << "'...";
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
      ranges::view::filter([&extension](const auto& p) {
        return p.path().extension() == extension;
      }) |
      ranges::view::transform([](const auto& p) {
        return LoadTextProto<Message>(p.path().string());
      });
  return messages;
}
}  // namespace

void ResourceManager::LoadResources(const Renderer& renderer) {
  LoadSprites();
  LoadAnimations();
  LoadTextures(renderer);
  LoadFonts(renderer);
}

void ResourceManager::CleanUp() {
  fonts_.clear();
  textures_.clear();
  scripts_.clear();
  sprites_.clear();
}

Scene ResourceManager::LoadScene(const std::string& scene_id) {
  return LoadTextProto<Scene>("../data/scenes/" + scene_id);
}

const Sprite& ResourceManager::GetSprite(const std::string& sprite_id) const {
  const auto it = sprites_.find(sprite_id);
  DLOG_IF(FATAL, it == sprites_.end()) << "Sprite with id='" << sprite_id
                                       << "' was not found.";
  return it->second;
}

const AnimationScript& ResourceManager::GetAnimationScript(
    const std::string& script_id) const {
  const auto it = scripts_.find(script_id);
  DLOG_IF(FATAL, it == scripts_.end()) << "AnimationScript with id='"
                                       << script_id << "' was not found.";
  return it->second;
}

const Texture& ResourceManager::GetTexture(
    const std::string& texture_id) const {
  const auto it = textures_.find(texture_id);
  DLOG_IF(FATAL, it == textures_.end()) << "Texture with id='" << texture_id
                                        << "' was not found.";
  return *it->second;
}

const Font& ResourceManager::GetFont(const std::string& font_id) const {
  const auto it = fonts_.find(font_id);
  DLOG_IF(FATAL, it == fonts_.end()) << "Font with id='" << font_id
                                     << "' was not found.";
  return *it->second;
}

void ResourceManager::LoadSprites() {
  const auto sprites =
      LoadTextProtoFromPath<Sprite>("../data/sprites/", ".sprite");

  sprites_ = sprites | ranges::view::transform([](const Sprite& sprite) {
               return std::make_pair(sprite.id(), sprite);
             });
}

void ResourceManager::LoadAnimations() {
  const auto animations =
      LoadTextProtoFromPath<SpriteAnimation>("../data/sprites/", ".animation");
  for (const auto& animation : animations) {
    ranges::action::insert(
        scripts_, animation.script() | ranges::view::transform([](
                                           const AnimationScript& script) {
                    return std::make_pair(script.id(), script);
                  }));
  }
}

void ResourceManager::LoadTextures(const Renderer& renderer) {
  std::vector<std::string> resources;
  std::transform(sprites_.begin(), sprites_.end(),
                 std::back_inserter(resources),
                 [](const auto& kv) { return kv.second.resource(); });
  std::sort(resources.begin(), resources.end());

  textures_ = resources | ranges::view::unique |
              ranges::view::transform([&renderer](const std::string& resource) {
                return std::make_pair(resource, renderer.LoadTexture(resource));
              });
}

constexpr char kDefaultFont[] = "fonts/times.ttf";

void ResourceManager::LoadFonts(const Renderer& renderer) {
  fonts_[kDefaultFont] = renderer.LoadFont(kDefaultFont, 16);
}

}  // namespace troll
