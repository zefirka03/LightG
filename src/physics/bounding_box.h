#pragma once
#include "../core/core.h"

struct boundingBox {
private:
	// a coordinates always must be less than b coordinates
	glm::vec3 a = glm::vec3(FLT_MAX);
	glm::vec3 b = glm::vec3(-FLT_MAX);

	glm::vec3 center;
	float diameter;
public:
	boundingBox() {}

	boundingBox(glm::vec3 _a, glm::vec3 _b) : a(_a), b(_b) {
		if (a.x > b.x) std::swap(a.x, b.x);
		if (a.y > b.y) std::swap(a.y, b.y);
		if (a.z > b.z) std::swap(a.z, b.z);

		center = (a + b) / 2.f;
		diameter = glm::length(b - a);
	}

	boundingBox(boundingBox const& other) {
		a = other.a;
		b = other.b;
		center = other.center;
		diameter = other.diameter;
	}

	glm::vec3 const& get_a() const {
		return a;
	}

	glm::vec3 const& get_b() const {
		return b;
	}

	glm::vec3 const& get_center() const {
		return center;
	}

	float get_diameter() const {
		return diameter;
	}

	void set_a(glm::vec3 const& _a) {
		a = _a;
		center = (a + b) / 2.f;
		diameter = glm::length(b - a);
	}

	void set_b(glm::vec3 const& _b)  {
		b = _b;
		center = (a + b) / 2.f;
		diameter = glm::length(b - a);
	}

	void set(glm::vec3 const& _a, glm::vec3 const& _b) {
		a = _a;
		b = _b;
		center = (a + b) / 2.f;
		diameter = glm::length(b - a);
	}

	void adjust_bounds(boundingBox const& other) {
		a.x = std::min(a.x, other.a.x);
		a.y = std::min(a.y, other.a.y);
		a.z = std::min(a.z, other.a.z);

		b.x = std::max(b.x, other.b.x);
		b.y = std::max(b.y, other.b.y);
		b.z = std::max(b.z, other.b.z);

		center = (a + b) / 2.f;
		diameter = glm::length(b - a);
	}

	// if this bbox fully contains other
	bool contains(boundingBox const& other) const {
		if (
			other.a.x >= a.x &&
			other.a.y >= a.y &&
			other.a.z >= a.z &&
			other.b.x <= b.x &&
			other.b.y <= b.y &&
			other.b.z <= b.z
		) return true;
		return false;
	}

	// if this bbox fully contains point
	bool contains(glm::vec3 const& point) const {
		if (
			point.x >= a.x &&
			point.y >= a.y &&
			point.z >= a.z &&
			point.x <= b.x &&
			point.y <= b.y &&
			point.z <= b.z
		) return true;
		return false;
	}

	// if this bbox intersect contains point
	bool intersect(boundingBox const& other) const {
		if (
			other.a.x <= b.x &&
			other.a.y <= b.y &&
			other.a.z <= b.z &&
			other.b.x >= a.x &&
			other.b.y >= a.y &&
			other.b.z >= a.z
		) return true;
		return false;
	}
};