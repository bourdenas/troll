#ifndef TROLL_CORE_RESOURCE_MANAGER_H_
#define TROLL_CORE_RESOURCE_MANAGER_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "proto/animation.pb.h"
#include "proto/key-binding.pb.h"
#include "proto/scene.pb.h"
#include "proto/sprite.pb.h"
#include "sdl/renderer.h"
#include "sdl/texture.h"
#include "sound/sound.h"

namespace troll {

class ResourceManager {
 public:
  static ResourceManager& Instance() {
    static ResourceManager singleton;
    return singleton;
  }

  void LoadResources(const Renderer& renderer);
  void CleanUp();

  Scene LoadScene(const std::string& scene);

  const KeyBindings& GetKeyBindings() const;
  const Sprite& GetSprite(const std::string& sprite_id) const;
  const boost::dynamic_bitset<>& GetSpriteCollisionMask(
      const std::string& sprite_id, int frame_index) const;
  const AnimationScript& GetAnimationScript(const std::string& script_id) const;
  const Texture& GetTexture(const std::string& texture_id) const;
  const Font& GetFont(const std::string& font_id) const;

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;

 private:
  ResourceManager() = default;
  ~ResourceManager() = default;

  void LoadAnimations();
  void LoadKeyBindings();
  void LoadSprites(const Renderer& renderer);
  void LoadTextures(const Renderer& renderer);
  void LoadFonts(const Renderer& renderer);
  void LoadSounds();

  KeyBindings key_bindings_;

  std::unordered_map<std::string, Sprite> sprites_;
  std::unordered_map<std::string, std::vector<boost::dynamic_bitset<>>>
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
