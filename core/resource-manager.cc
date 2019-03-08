#include "core/resource-manager.h"

#include <experimental/filesystem>
#include <fstream>

#include <absl/strings/str_cat.h>
#include <glog/logging.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
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
  const auto resource_path = std::experimental::filesystem::path(path);

  std::vector<Message> messages;
  for (const auto& p :
       std::experimental::filesystem::directory_iterator(resource_path)) {
    if (p.path().extension() != extension) continue;

    messages.push_back(LoadTextProto<Message>(p.path().string()));
  }
  return messages;
}
}  // namespace

void ResourceManager::LoadResources(const std::string& base_path,
                                    const Renderer* renderer,
                                    const SoundLoader* sound_loader) {
  LoadAnimations(base_path);
  LoadKeyBindings(base_path);
  LoadSprites(base_path, renderer);
  LoadTextures(base_path, renderer);
  LoadFonts(base_path);
  LoadSounds(base_path, sound_loader);
}

Scene ResourceManager::LoadScene(const std::string& filename) {
  return LoadTextProto<Scene>(filename);
}

const KeyBindings& ResourceManager::GetKeyBindings() const {
  return key_bindings_;
}

const Sprite& ResourceManager::GetSprite(const std::string& sprite_id) const {
  const auto it = sprites_.find(sprite_id);
  LOG_IF(FATAL, it == sprites_.end())
      << "Sprite with id='" << sprite_id << "' was not found.";
  return it->second;
}

const std::vector<bool>& ResourceManager::GetSpriteCollisionMask(
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
  LOG_IF(FATAL, it == scripts_.end())
      << "AnimationScript with id='" << script_id << "' was not found.";
  return it->second;
}

const Texture& ResourceManager::GetTexture(
    const std::string& texture_id) const {
  const auto it = textures_.find(texture_id);
  LOG_IF(FATAL, it == textures_.end())
      << "Texture with id='" << texture_id << "' was not found.";
  return *it->second;
}

const Font& ResourceManager::GetFont(const std::string& font_id) const {
  const auto it = fonts_.find(font_id);
  LOG_IF(FATAL, it == fonts_.end())
      << "Font with id='" << font_id << "' was not found.";
  return *it->second;
}

const Music& ResourceManager::GetMusic(const std::string& track_id) const {
  const auto it = music_tracks_.find(track_id);
  LOG_IF(FATAL, it == music_tracks_.end())
      << "Music track with id='" << track_id << "' was not found.";
  return *it->second;
}

const Sound& ResourceManager::GetSound(const std::string& sfx_id) const {
  const auto it = sfx_.find(sfx_id);
  LOG_IF(FATAL, it == sfx_.end())
      << "Sound effect with id='" << sfx_id << "' was not found.";
  return *it->second;
}

void ResourceManager::LoadAnimations(const std::string& base_path) {
  const auto animations = LoadTextProtoFromPath<SpriteAnimation>(
      absl::StrCat(base_path, "sprites/"), ".animation");
  for (const auto& animation : animations) {
    ranges::action::insert(
        scripts_,
        animation.script() |
            ranges::view::transform([](const AnimationScript& script) {
              return std::make_pair(script.id(), script);
            }));
  }
}

void ResourceManager::LoadKeyBindings(const std::string& base_path) {
  const auto key_bindings = LoadTextProtoFromPath<KeyBindings>(
      absl::StrCat(base_path, "scenes/"), ".keys");
  for (const auto& keys : key_bindings) {
    key_bindings_.MergeFrom(keys);
  }
}

void ResourceManager::LoadSprites(const std::string& base_path,
                                  const Renderer* renderer) {
  const auto sprites = LoadTextProtoFromPath<Sprite>(
      absl::StrCat(base_path, "sprites/"), ".sprite");

  sprites_ = sprites | ranges::view::transform([](const Sprite& sprite) {
               return std::make_pair(sprite.id(), sprite);
             });
  sprite_collision_masks_ =
      sprites_ | ranges::view::values |
      ranges::view::transform([&base_path, renderer](const Sprite& sprite) {
        return std::make_pair(
            sprite.id(), renderer->GenerateCollisionMasks(
                             absl::StrCat(base_path, "resources/"), sprite));
      });
}

void ResourceManager::LoadTextures(const std::string& base_path,
                                   const Renderer* renderer) {
  std::vector<std::pair<std::string, std::string>> resources =
      sprites_ | ranges::view::values |
      ranges::view::transform([](const Sprite& sprite) {
        return std::make_pair(sprite.resource(),
                              sprite.colour_key().SerializeAsString());
      });
  std::sort(resources.begin(), resources.end());

  textures_ =
      resources | ranges::view::unique |
      ranges::view::transform([&base_path, renderer](const auto& resource) {
        RGBa colour_key;
        colour_key.ParseFromString(resource.second);
        return std::make_pair(
            resource.first,
            Texture::CreateTextureFromFile(
                absl::StrCat(base_path, "resources/", resource.first),
                colour_key, renderer));
      });
}

namespace {
constexpr char kDefaultFont[] = "fonts/times.ttf";
}  // namespace

void ResourceManager::LoadFonts(const std::string& base_path) {
  fonts_[kDefaultFont] = Font::CreateFontFromFile(
      absl::StrCat(base_path, "resources/", kDefaultFont), 16);
}

void ResourceManager::LoadSounds(const std::string& base_path,
                                 const SoundLoader* sound_loader) {
  const auto sounds =
      LoadTextProtoFromPath<Audio>(absl::StrCat(base_path, "scenes/"), ".sfx");

  for (const Audio& sound : sounds) {
    for (const auto& track : sound.track()) {
      music_tracks_.emplace(
          track.id(), sound_loader->LoadMusic(absl::StrCat(
                          base_path, "resources/sounds/", track.resource())));
    }
    for (const auto& sfx : sound.sfx()) {
      sfx_.emplace(sfx.id(),
                   sound_loader->LoadSound(absl::StrCat(
                       base_path, "resources/sounds/", sfx.resource())));
    }
  }
}

}  // namespace troll
