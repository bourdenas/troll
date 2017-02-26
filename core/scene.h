#ifndef TROLL_CORE_SCENE_H_
#define TROLL_CORE_SCENE_H_

#include <map>
#include <memory>
#include <string>

#include "event.h"
#include "world-element.h"

namespace troll {

class SceneNode;

class SceneNodeContainer {
  const std::map<std::string, std::unique_ptr<SceneNode>>& nodes() {
    return nodes_;
  }
  std::map<std::string, std::unique_ptr<SceneNode>>* mutable_nodes() {
    return &nodes_;
  }

 private:
  std::map<std::string, std::unique_ptr<SceneNode>> nodes_;
};

// A SceneNode is an instance of a movable/interactable element in the world.
// It represents a tangible element of the game that has dimensions defined by
// a bounding box.
class SceneNode : public WorldElement, public SceneNodeContainer {
 public:
  SceneNode(const std::string& id) : WorldElement(id) {}
  ~SceneNode() override = default;

  // virtual bool IsInside(const Vector& point) const = 0;
  virtual bool CollidesWith(SceneNode* node) const = 0;
  virtual void OnCollision(SceneNode* node) = 0;

  // virtual void Position(const Vector& vec) = 0;
  // virtual void Move(const Vector& vec) = 0;
  // virtual void Rotate(const Vector& vec) = 0;
  // virtual void Scale(const Vector& vec) = 0;

  virtual void SetResource(const std::string& id);

  virtual const BoundingBox& bounding_box() const = 0;
  const Object& object() const { return *object_; }

  bool visible() const { return visible_; }
  void set_visible(bool flag) { visible_ = flag; }

  const SceneNodeContainer& parent() const { return *parent_; }
  void set_parent(const SceneNodeContainer* parent) { parent_ = parent; }

 private:
  bool visible_;
  const Object* object_;              // Not owned.
  const SceneNodeContainer* parent_;  // Not owned, used for late destroy.
};

// A character is a special object in the world, that represents the player
// avatar.
class Character : public SceneNode {
 public:
  Character(const std::string& id) : SceneNode(id) {}
  ~Character() override = default;

  const std::string& scene() const { return scene_; }
  void set_scene(const std::string& scene) { scene_ = scene; }

 private:
  // Scene that character is currently lying.
  std::string scene_;
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
