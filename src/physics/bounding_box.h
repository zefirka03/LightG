#pragma once
#include "../core/core.h"

struct boundingBox {
	// a coordinates always must be less than b coordinates
	glm::vec3 a = glm::vec3(FLT_MAX);
	glm::vec3 b = glm::vec3(-FLT_MAX);

	boundingBox() {}

	boundingBox(glm::vec3 _a, glm::vec3 _b) : a(_a), b(_b) {
		if (a.x > b.x) std::swap(a.x, b.x);
		if (a.y > b.y) std::swap(a.y, b.y);
		if (a.z > b.z) std::swap(a.z, b.z);

		a -= glm::vec3(0.05);
		b += glm::vec3(0.05);
	}

	void adjust_bounds(boundingBox const& other) {
		a.x = std::min(a.x, other.a.x);
		a.y = std::min(a.y, other.a.y);
		a.z = std::min(a.z, other.a.z);

		b.x = std::max(b.x, other.b.x);
		b.y = std::max(b.y, other.b.y);
		b.z = std::max(b.z, other.b.z);
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
};