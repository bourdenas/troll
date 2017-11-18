#ifndef TROLL_CORE_SCENE_H_
#define TROLL_CORE_SCENE_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "core/event.h"
#include "core/world-element.h"
#include "proto/primitives.pb.h"
#include "proto/scene.pb.h"

namespace troll {

class SceneNode;

class SceneNodeContainer {
 public:
  SceneNodeContainer() = default;
  virtual ~SceneNodeContainer() = default;

  const std::unordered_map<std::string, std::unique_ptr<SceneNode>>& nodes() {
    return nodes_;
  }
  std::unordered_map<std::string, std::unique_ptr<SceneNode>>* mutable_nodes() {
    return &nodes_;
  }

 private:
  std::unordered_map<std::string, std::unique_ptr<SceneNode>> nodes_;
};

// A SceneNode is an instance of a movable/interactable element in the world.
// It represents a tangible element of the game that has dimensions defined by
// a bounding box.
class SceneNode : public WorldElement, public SceneNodeContainer {
 public:
  explicit SceneNode(const std::string& id) : WorldElement(id) {}
  ~SceneNode() override = default;

  bool IsInside(const Vector& point) const;
  bool CollidesWith(const SceneNode& other) const;

  void Position(const Vector& vec);
  void Translate(const Vector& vec);
  void Rotate(const Vector& vec);
  void Scale(const Vector& vec);

  void Display(void) const;

  const std::string& sprite_id() const { return sprite_id_; }
  void set_frame(int index) { frame_index_ = index; }
  const Box& bounding_box() const;

  bool visible() const { return visible_; }
  void set_visible(bool flag) { visible_ = flag; }

 private:
  const std::string sprite_id_;
  int frame_index_;
  bool visible_;
};

// A character is a special object in the world, that represents the player
// avatar.
class Character : public SceneNode {
 public:
  explicit Character(const std::string& id) : SceneNode(id) {}
  ~Character() override = default;
};

}  // namespace troll

#endif  // TROLL_CORE_SCENE_H_
