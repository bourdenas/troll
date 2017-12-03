#ifndef TROLL_CORE_RESOURCE_MANAGER_H_
#define TROLL_CORE_RESOURCE_MANAGER_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "proto/animation.pb.h"
#include "proto/scene.pb.h"
#include "proto/sprite.pb.h"
#include "sdl/renderer.h"
#include "sdl/texture.h"

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

  const Sprite& GetSprite(const std::string& sprite_id) const;
  const AnimationScript& GetAnimationScript(const std::string& script_id) const;
  const Texture& GetTexture(const std::string& texture_id) const;
  const Font& GetFont(const std::string& font_id) const;

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;

 private:
  ResourceManager() = default;
  virtual ~ResourceManager() = default;

  void LoadSprites();
  void LoadAnimations();
  void LoadTextures(const Renderer& renderer);
  void LoadFonts(const Renderer& renderer);

  std::unordered_map<std::string, Sprite> sprites_;
  std::unordered_map<std::string, AnimationScript> scripts_;
  std::unordered_map<std::string, std::unique_ptr<Texture>> textures_;
  std::unordered_map<std::string, std::unique_ptr<Font>> fonts_;
};

}  // namespace troll

#endif  // TROLL_CORE_RESOURCE_MANAGER_H_
