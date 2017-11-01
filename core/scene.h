#ifndef TROLL_CORE_SCENE_H_
#define TROLL_CORE_SCENE_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "core/event.h"
#include "core/primitives.h"
#include "core/world-element.h"

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
  SceneNode(const std::string& id) : WorldElement(id) {}
  ~SceneNode() override = default;

  bool IsInside(const Point& point) const;
  bool CollidesWith(SceneNode* node) const;
  void OnCollision(SceneNode* node);

  void Position(const Point& vec);
  void Move(const Point& vec);
  void Rotate(const Point& vec);
  void Scale(const Point& vec);

  const Box& bounding_box() const;
  const std::string& object_id() const { return object_id_; }

  bool visible() const { return visible_; }
  void set_visible(bool flag) { visible_ = flag; }

 private:
  const std::string object_id_;
  bool visible_;
};

// A character is a special object in the world, that represents the player
// avatar.
class Character : public SceneNode {
 public:
  explicit Character(const std::string& id) : SceneNode(id) {}
  ~Character() override = default;
};

// A scene of the world. It contains SceneNodes that together compose the
// player environment.
class Scene : public WorldElement,
              public SceneNodeContainer,
              public EventHandler {
 public:
  Scene(const std::string& id) : WorldElement(id) {}
  ~Scene() override = default;
};

}  // namespace troll

#endif  // TROLL_CORE_SCENE_H_
