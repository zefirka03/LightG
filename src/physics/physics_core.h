#pragma once
#include "../core/core.h"
#include "../debug/debug.h"
#include "../render/transform.h"

struct boundingBox {
	// a coordinates always must be less than b coordinates
	glm::vec3 a;
	glm::vec3 b;

	boundingBox() : a(0), b(0) {}
	boundingBox(glm::vec3 _a, glm::vec3 _b) : a(_a), b(_b) {}

	void adjust_bounds(boundingBox const& other) {
		a.x = std::min(a.x, other.a.x);
		a.y = std::min(a.y, other.a.y);
		a.z = std::min(a.z, other.a.z);

		b.x = std::max(b.x, other.b.x);
		b.y = std::max(b.y, other.b.y);
		b.z = std::max(b.z, other.b.z);
	}
};

class Collider {
public:
	virtual boundingBox get_bounds() const = 0;
	virtual void update_transform(Transform& transform) = 0;

	virtual void draw_debug(DebugSystem& debug_system) const {}
};

struct spriteTransform {
	glm::vec2 size;
	glm::vec2 origin;
	float angle;
};

class SpriteCollider : public Collider {
public:
	glm::vec2 size;
	glm::vec2 origin;
	Transform worldTransform;

	SpriteCollider() {}

	boundingBox get_bounds() const override {
		glm::mat4 displace = glm::eulerAngleXYZ(0.f, worldTransform.rotation.y, 0.f);
		displace = glm::translate(displace, glm::vec3(worldTransform.position));

		glm::vec3 a1 = displace * glm::vec4(glm::vec2(0, 0) - origin, 0, 1);
		glm::vec3 a2 = displace * glm::vec4(glm::vec2(size.x, 0) - origin, 0, 1);
		glm::vec3 a3 = displace * glm::vec4(glm::vec2(size.x, size.y) - origin, 0, 1);
		glm::vec3 a4 = displace * glm::vec4(glm::vec2(0, size.y) - origin, 0, 1);

		glm::vec3 a = glm::vec3(
			std::min(a1.x, a2.x),
			std::min(a1.y, a2.y),
			std::min(a1.z, a2.z)
		);

		glm::vec3 b = glm::vec3(
			std::max(a3.x, a4.x),
			std::max(a3.y, a4.y),
			std::max(a3.z, a4.z)
		);

		return boundingBox(a, b);
	}

	void update_transform(Transform& transform) override {
		worldTransform = transform;
	}

	void draw_debug(DebugSystem& debug_system) const override {
		glm::mat4 displace = glm::eulerAngleXYZ(0.f, worldTransform.rotation.y, 0.f);
		displace = glm::translate(displace, glm::vec3(worldTransform.position));

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