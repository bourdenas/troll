#ifndef TROLL_CORE_RESOURCE_MANAGER_H_
#define TROLL_CORE_RESOURCE_MANAGER_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "proto/animation.pb.h"
#include "proto/key-binding.pb.h"
#include "proto/scene.pb.h"
#include "proto/sprite.pb.h"
#include "sdl/texture.h"
#include "sound/sound.h"

namespace troll {

class Renderer;
class SoundLoader;

class ResourceManager {
 public:
  ResourceManager() = default;
  ~ResourceManager() = default;

  void LoadResources(const std::string& base_path, const Renderer* renderer,
                     const SoundLoader* sound_loader);

  Scene LoadScene(const std::string& filename);

  const KeyBindings& GetKeyBindings() const;

  const Sprite& GetSprite(const std::string& sprite_id) const;
  const std::vector<bool>& GetSpriteCollisionMask(const std::string& sprite_id,
                                                  int frame_index) const;

  const AnimationScript& GetAnimationScript(const std::string& script_id) const;

  const Texture& GetTexture(const std::string& texture_id) const;
  const Font& GetFont(const std::string& font_id) const;

  const Music& GetMusic(const std::string& track_id) const;
  const Sound& GetSound(const std::string& sfx_id) const;

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;

 private:
  void LoadAnimations(const std::string& base_path);
  void LoadKeyBindings(const std::string& base_path);
  void LoadSprites(const std::string& base_path, const Renderer* renderer);
  void LoadTextures(const std::string& base_path, const Renderer* renderer);
  void LoadFonts(const std::string& base_path);
  void LoadSounds(const std::string& base_path,
                  const SoundLoader* sound_loader);

  KeyBindings key_bindings_;

  std::unordered_map<std::string, Sprite> sprites_;
  std::unordered_map<std::string, std::vector<std::vector<bool>>>
      sprite_collision_masks_;

  std::unordered_map<std::string, AnimationScript> scripts_;

  std::unordered_map<std::string, std::unique_ptr<Texture>> textures_;
  std::unordered_map<std::string, std::unique_ptr<Font>> fonts_;

  std::unordered_map<std::string, std::unique_ptr<Music>> music_tracks_;
  std::unordered_map<std::string, std::unique_ptr<Sound>> sfx_;

  friend class TestingResourceManager;
};

}  // namespace troll

#endif  // TROLL_CORE_RESOURCE_MANAGER_H_
