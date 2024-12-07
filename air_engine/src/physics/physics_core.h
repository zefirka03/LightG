#pragma once
#include "../core/core.h"
#include "../debug/debug.h"
#include "../render/transform.h"
#include "quadtree.h"
#include "ray.h"
#include "line.h"

struct collisionData {
	bool is_collide = false;
	glm::vec3 collision_point;
	glm::vec3 normal;
	float distanse;

	collisionData() {}

	collisionData(
		bool is_collide,
		glm::vec3 collision_point, 
		glm::vec3 normal, 
		float distanse
	) : is_collide(is_collide),
		collision_point(collision_point), 
		normal(normal), 
		distanse(distanse) {}
};

struct rayIntersection{
	std::vector<collisionData> points;
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

	virtual void check_collision(Collider* other, collisionData& out) = 0;
	virtual void draw_debug(DebugSystem& debug_system) const {}
	virtual void ray_intersect(Ray const& ray, rayIntersection& out) const {}

	virtual ~Collider() {}
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

/* 
gets collisionData 
normal should be from a to b
*/
struct CollisionCheckers {
	static void is_collide(SpriteCollider* a, SpriteCollider* b, collisionData& out);
	// Always false
	static void is_collide(PlaneCollider* a, PlaneCollider* b, collisionData& out);
	// Not implemented yet
	static void is_collide(SpriteCollider* a, PlaneCollider* b, collisionData& out);
	static void is_collide(SphereCollider* a, PlaneCollider* b, collisionData& out);
	static void is_collide(SphereCollider* a, SpriteCollider* b, collisionData& out);
	static void is_collide(SphereCollider* a, SphereCollider* b, collisionData& out);
};

class SpriteCollider : public Collider {
public:
	glm::vec2 size;
	glm::vec2 origin;

	SpriteCollider();

	void update_bounds() override;
	void check_collision(Collider* other, collisionData& out) override;
	void draw_debug(DebugSystem& debug_system) const override;
	void ray_intersect(Ray const& ray, rayIntersection& out) const override;

	~SpriteCollider() {}
};

class PlaneCollider : public Collider {
public:
	glm::vec2 size;
	glm::vec2 origin;

	PlaneCollider();

	void update_bounds() override;
	void check_collision(Collider* other, collisionData& out) override;
	void draw_debug(DebugSystem& debug_system) const override;
	void ray_intersect(Ray const& ray, rayIntersection& out) const override;

	~PlaneCollider() {}
};

class SphereCollider : public Collider {
public:
	float radius;

	SphereCollider();

	void update_bounds() override;
	void check_collision(Collider* other, collisionData& out) override;
	void draw_debug(DebugSystem& debug_system) const override;
	void ray_intersect(Ray const& ray, rayIntersection& out) const override;

	~SphereCollider() {}
};