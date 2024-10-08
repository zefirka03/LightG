#pragma once
#include "../core/core.h"
#include "../debug/debug.h"
#include "../render/transform.h"
#include "quadtree.h"
#include "line.h"

struct collisionData {
	bool is_collide;
	glm::vec3 collision_point;
	glm::vec3 normal;
	float distanse;
};

struct Collider : public Quadable {
private:
friend class PhysicsBody;
friend class PhysicsSystem;
	PhysicsBody* m_pb_handler = nullptr;
public:
	virtual void update_transform(Transform& transform) = 0;
	virtual collisionData check_collision(Collider* other) = 0;

	virtual void draw_debug(DebugSystem& debug_system) const {}
};

struct spriteTransform {
	glm::vec2 size;
	glm::vec2 origin;
	float angle;
};

class SpriteCollider;
class PlaneCollider;
class SphereCollider;

struct CollisionCheckers {
	static collisionData is_collide(SpriteCollider* a, SpriteCollider* b);
	// Always false
	static collisionData is_collide(PlaneCollider* a, PlaneCollider* b);
	// Not implemented yet
	static collisionData is_collide(SpriteCollider* a, PlaneCollider* b);
	static collisionData is_collide(SphereCollider* a, PlaneCollider* b);
	static collisionData is_collide(SphereCollider* a, SpriteCollider* b);
};

class SpriteCollider : public Collider {
public:
	glm::vec2 size;
	glm::vec2 origin;
	Transform world_transform;

	SpriteCollider();

	boundingBox get_bounds() const override;
	collisionData check_collision(Collider* other) override;
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
	collisionData check_collision(Collider* other) override;
	void update_transform(Transform& transform) override;
	void draw_debug(DebugSystem& debug_system) const override;
};

class SphereCollider : public Collider {
public:
	float radius;
	Transform world_transform;

	SphereCollider();

	boundingBox get_bounds() const override;
	collisionData check_collision(Collider* other) override;
	void update_transform(Transform& transform) override;
	void draw_debug(DebugSystem& debug_system) const override;
};