#pragma once
#include "../core/core.h"
#include "../debug/debug.h"
#include "../render/transform.h"
#include "quadtree.h"

struct Collider : public Quadable {
private:
friend class PhysicsBody;
friend class PhysicsSystem;
	PhysicsBody* m_pb_handler = nullptr;
public:
	virtual void update_transform(Transform& transform) = 0;
	virtual bool check_collision(Collider* other) = 0;

	virtual void draw_debug(DebugSystem& debug_system) const {}
};

struct spriteTransform {
	glm::vec2 size;
	glm::vec2 origin;
	float angle;
};

class SpriteCollider;
class PlaneCollider;

struct CollisionCheckers {
	static bool is_collide(SpriteCollider* a, SpriteCollider* b);
	static bool is_collide(PlaneCollider* a, PlaneCollider* b);
	static bool is_collide(SpriteCollider* a, PlaneCollider* b);
};

class SpriteCollider : public Collider {
public:
	glm::vec2 size;
	glm::vec2 origin;
	Transform world_transform;

	SpriteCollider();

	boundingBox get_bounds() const override;
	bool check_collision(Collider* other) override;
	void update_transform(Transform& transform) override;
	void draw_debug(DebugSystem& debug_system) const override;
};

class PlaneCollider : public Collider {
public:
	glm::vec2 size;
	glm::vec2 origin;
	Transform world_transform;

	PlaneCollider();

	boundingBox get_bounds() const override;
	bool check_collision(Collider* other) override;
	void update_transform(Transform& transform) override;
	void draw_debug(DebugSystem& debug_system) const override;
};