#pragma once
#include "../core/core.h"
#include "../debug/debug.h"
#include "../render/transform.h"
#include "quadtree.h"
#include "line.h"

struct collisionData {
	bool is_collide = false;
	glm::vec3 collision_point;
	glm::vec3 normal;
	float distanse;
};

template<typename T>
constexpr int check_collider_type();

struct Collider : public Quadable {
protected:
friend class PhysicsBody;
friend class PhysicsSystem;
friend class CollisionCheckers;
	PhysicsBody* m_pb_handler = nullptr;
	Transform* m_transform_handler = nullptr;
	uint32_t m_cached_type = check_collider_type<Collider>();

	bool m_cached_already_resolved = false;
public:
	template<class Collider_t>
	Collider_t* cached_dynamic_cast() {
		if (check_collider_type<Collider_t>() == m_cached_type)
			return static_cast<Collider_t*>(this);
		return nullptr;
	}

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

template<typename T>
constexpr int check_collider_type() {
	if constexpr (std::is_same<T, Collider>::value)
		return -1;
	else if constexpr (std::is_same<T, SpriteCollider>::value)
		return 0;
	else if constexpr (std::is_same<T, PlaneCollider>::value) 
		return 1;
	else if constexpr (std::is_same<T, SphereCollider>::value) 
		return 2;
	else 
		return 3;
}

struct CollisionCheckers {
	static collisionData is_collide(SpriteCollider* a, SpriteCollider* b);
	// Always false
	static collisionData is_collide(PlaneCollider* a, PlaneCollider* b);
	// Not implemented yet
	static collisionData is_collide(SpriteCollider* a, PlaneCollider* b);
	static collisionData is_collide(SphereCollider* a, PlaneCollider* b);
	static collisionData is_collide(SphereCollider* a, SpriteCollider* b);
	static collisionData is_collide(SphereCollider* a, SphereCollider* b);
};

class SpriteCollider : public Collider {
public:
	glm::vec2 size;
	glm::vec2 origin;

	SpriteCollider();

	void update_bounds() override;
	collisionData check_collision(Collider* other) override;
	void draw_debug(DebugSystem& debug_system) const override;
};

class PlaneCollider : public Collider {
public:
	glm::vec2 size;
	glm::vec2 origin;

	PlaneCollider();

	void update_bounds() override;
	collisionData check_collision(Collider* other) override;
	void draw_debug(DebugSystem& debug_system) const override;
};

class SphereCollider : public Collider {
public:
	float radius;

	SphereCollider();

	void update_bounds() override;
	collisionData check_collision(Collider* other) override;
	void draw_debug(DebugSystem& debug_system) const override;
};