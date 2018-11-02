#include "core/resource-manager.h"

#include <fstream>

#include <glog/logging.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <boost/filesystem.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/map.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/unique.hpp>

#include "proto/animation.pb.h"
#include "proto/key-binding.pb.h"
#include "proto/scene.pb.h"
#include "proto/sprite.pb.h"
#include "sdl/renderer.h"
#include "sound/sound-loader.h"

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
  LoadAnimations();
  LoadKeyBindings();
  LoadSprites(renderer);
  LoadTextures(renderer);
  LoadFonts(renderer);
  LoadSounds();
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

const KeyBindings& ResourceManager::GetKeyBindings() const {
  return key_bindings_;
}

const Sprite& ResourceManager::GetSprite(const std::string& sprite_id) const {
  const auto it = sprites_.find(sprite_id);
  LOG_IF(FATAL, it == sprites_.end()) << "Sprite with id='" << sprite_id
                                      << "' was not found.";
  return it->second;
}

const boost::dynamic_bitset<>& ResourceManager::GetSpriteCollisionMask(
    const std::string& sprite_id, int frame_index) const {
  const auto it = sprite_collision_masks_.find(sprite_id);
  LOG_IF(FATAL, it == sprite_collision_masks_.end())
      << "Sprite collision mask with id='" << sprite_id << "' was not found.";

  LOG_IF(FATAL, frame_index >= it->second.size())
      << "Frame index " << frame_index << " out of bounds for sprite '"
      << sprite_id << "'.";
  return it->second[frame_index];
}

const AnimationScript& ResourceManager::GetAnimationScript(
    const std::string& script_id) const {
  const auto it = scripts_.find(script_id);
  LOG_IF(FATAL, it == scripts_.end()) << "AnimationScript with id='"
                                      << script_id << "' was not found.";
  return it->second;
}

const Texture& ResourceManager::GetTexture(
    const std::string& texture_id) const {
  const auto it = textures_.find(texture_id);
  LOG_IF(FATAL, it == textures_.end()) << "Texture with id='" << texture_id
                                       << "' was not found.";
  return *it->second;
}

const Font& ResourceManager::GetFont(const std::string& font_id) const {
  const auto it = fonts_.find(font_id);
  LOG_IF(FATAL, it == fonts_.end()) << "Font with id='" << font_id
                                    << "' was not found.";
  return *it->second;
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

void ResourceManager::LoadKeyBindings() {
  const auto key_bindings =
      LoadTextProtoFromPath<KeyBindings>("../data/scenes/", ".keys");
  for (const auto& keys : key_bindings) {
    key_bindings_.MergeFrom(keys);
  }
}

void ResourceManager::LoadSprites(const Renderer& renderer) {
  const auto sprites =
      LoadTextProtoFromPath<Sprite>("../data/sprites/", ".sprite");

  sprites_ = sprites | ranges::view::transform([](const Sprite& sprite) {
               return std::make_pair(sprite.id(), sprite);
             });
  sprite_collision_masks_ =
      sprites_ | ranges::view::values |
      ranges::view::transform([&renderer](const Sprite& sprite) {
        return std::make_pair(sprite.id(),
                              renderer.GenerateCollisionMasks(sprite));
      });
}

void ResourceManager::LoadTextures(const Renderer& renderer) {
  std::vector<std::pair<std::string, std::string>> resources =
      sprites_ | ranges::view::values |
      ranges::view::transform([](const Sprite& sprite) {
        return std::make_pair(sprite.resource(),
                              sprite.colour_key().SerializeAsString());
      });
  std::sort(resources.begin(), resources.end());

  textures_ =
      resources | ranges::view::unique |
      ranges::view::transform([&renderer](const auto& resource) {
        RGBa colour_key;
        colour_key.ParseFromString(resource.second);
        return std::make_pair(resource.first,
                              renderer.LoadTexture(resource.first, colour_key));
      });
}

constexpr char kDefaultFont[] = "fonts/times.ttf";

void ResourceManager::LoadFonts(const Renderer& renderer) {
  fonts_[kDefaultFont] = renderer.LoadFont(kDefaultFont, 16);
}

void ResourceManager::LoadSounds() {
  const auto sounds = LoadTextProtoFromPath<Audio>("../data/scenes/", ".sfx");

  for (const Audio& sound : sounds) {
    for (const auto& track : sound.track()) {
      music_tracks_.emplace(
          track.id(), SoundLoader::Instance().LoadMusic(track.resource()));
    }
    for (const auto& sfx : sound.sfx()) {
      sfx_.emplace(sfx.id(), SoundLoader::Instance().LoadSound(sfx.resource()));
    }
  }
}

}  // namespace troll
