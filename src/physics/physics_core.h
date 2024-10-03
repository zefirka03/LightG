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

struct CollisionCheckers {
	static bool is_collide(SpriteCollider* a, SpriteCollider* b);
};

class SpriteCollider : public Collider {
public:
	glm::vec2 size;
	glm::vec2 origin;
	Transform world_transform;

	SpriteCollider() {}

	boundingBox get_bounds() const override {
		return boundingBox(world_transform.position + glm::vec3(
			(-origin.x) * cos(world_transform.rotation.y),
			-origin.y,
			(-origin.x) * sin(world_transform.rotation.y)
		), world_transform.position + glm::vec3(
			(size.x - origin.x) * cos(world_transform.rotation.y),
			size.y - origin.y,
			(size.x - origin.x) * sin(world_transform.rotation.y)
		));
	}

	bool check_collision(Collider* other) override {
		if (dynamic_cast<SpriteCollider*>(other))
			return CollisionCheckers::is_collide(this, static_cast<SpriteCollider*>(other));
		return false;
	}

	void update_transform(Transform& transform) override {
		world_transform = transform;
	}

	void draw_debug(DebugSystem& debug_system) const override {
		glm::mat4 displace = glm::translate(glm::mat4(1), glm::vec3(world_transform.position)) * glm::eulerAngleXYZ(0.f, world_transform.rotation.y, 0.f);

		glm::vec3 a1 = displace * glm::vec4(glm::vec2(0, 0) - origin, 0, 1);
		glm::vec3 a2 = displace * glm::vec4(glm::vec2(size.x, 0) - origin, 0, 1) ;
		glm::vec3 a3 = displace * glm::vec4(glm::vec2(size.x, size.y) - origin, 0, 1) ;
		glm::vec3 a4 = displace * glm::vec4(glm::vec2(0, size.y) - origin, 0, 1);

		debug_system.draw_line({ {a1, glm::vec4(1)}, {a2, glm::vec4(1)} });
		debug_system.draw_line({ {a2, glm::vec4(1)}, {a3, glm::vec4(1)} });
		debug_system.draw_line({ {a3, glm::vec4(1)}, {a4, glm::vec4(1)} });
		debug_system.draw_line({ {a4, glm::vec4(1)}, {a1, glm::vec4(1)} });
	}
};

bool CollisionCheckers::is_collide(SpriteCollider* a, SpriteCollider* b) {
	glm::vec3 A_a = a->world_transform.position + glm::vec3(
		(-a->origin.x) * cos(a->world_transform.rotation.y),
		-a->origin.y,
		(-a->origin.x) * sin(a->world_transform.rotation.y)
	);

	glm::vec3 A_b = a->world_transform.position + glm::vec3(
		(a->size.x - a->origin.x) * cos(a->world_transform.rotation.y),
		a->size.y - a->origin.y,
		(a->size.x - a->origin.x) * sin(a->world_transform.rotation.y)
	);

	glm::vec3 B_a = b->world_transform.position + glm::vec3(
		(-b->origin.x) * cos(b->world_transform.rotation.y),
		-b->origin.y,
		(-b->origin.x) * sin(b->world_transform.rotation.y)
	);

	glm::vec3 B_b = b->world_transform.position + glm::vec3(
		(b->size.x - b->origin.x) * cos(b->world_transform.rotation.y),
		b->size.y - b->origin.y,
		(b->size.x - b->origin.x) * sin(b->world_transform.rotation.y)
	);


	boundingBox A(A_a, A_b);
	boundingBox B(B_a, B_b);

	glm::mat2 det = glm::mat2({ 
		(A_b.z - A_a.z), -(A_b.x - A_a.x),
		(B_b.z - B_a.z), -(B_b.x - B_a.x)
	});

	glm::mat2 x1 = glm::mat2({ 
		A_a.x * (A_b.z - A_a.z) - A_a.z * (A_b.x - A_a.x), -(A_b.x - A_a.x),
		B_a.x * (B_b.z - B_a.z) - B_a.z * (B_b.x - B_a.x), -(B_b.x - B_a.x)
	});

	glm::mat2 z1 = glm::mat2({
		(A_b.z - A_a.z), A_a.x * (A_b.z - A_a.z) - A_a.z * (A_b.x - A_a.x),
		(B_b.z - B_a.z), B_a.x * (B_b.z - B_a.z) - B_a.z * (B_b.x - B_a.x)
	});

	glm::vec2 xz = { glm::determinant(x1) / glm::determinant(det), glm::determinant(z1) / glm::determinant(det) };

	float y1 = std::min(A_b.y, B_b.y);
	float y2 = std::max(A_a.y, B_a.y);
	
	glm::vec3 xyz = { xz.x, (y2 + y1) / 2.f, xz.y};

	return A.contains(xyz) && B.contains(xyz);
}