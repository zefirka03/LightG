#include "physics_core.h"

PlaneCollider::PlaneCollider() {}

// incorrect
void PlaneCollider::update_bounds() {
	m_bbox = boundingBox(m_transform_handler->position + glm::vec3(
		(-origin.x) * cos(m_transform_handler->rotation.y),
		-0.01,
		(-origin.y) * cos(m_transform_handler->rotation.y)
	), m_transform_handler->position + glm::vec3(
		(size.x - origin.x) * cos(m_transform_handler->rotation.y),
		0.01,
		(size.y - origin.y) * cos(m_transform_handler->rotation.y)
	));
}

void PlaneCollider::check_collision(Collider* other, collisionData& out) {
	if (other->cached_dynamic_cast<PlaneCollider>())
		return CollisionCheckers::is_collide(this, static_cast<PlaneCollider*>(other), out);
	else if (other->cached_dynamic_cast<SpriteCollider>())
		return CollisionCheckers::is_collide(static_cast<SpriteCollider*>(other), this, out);
	else if (other->cached_dynamic_cast<SphereCollider>())
		return CollisionCheckers::is_collide(static_cast<SphereCollider*>(other), this, out);
	return;
}

void PlaneCollider::draw_debug(DebugSystem& debug_system) const {
	glm::mat4 displace = glm::translate(glm::mat4(1), glm::vec3(m_transform_handler->position)) * glm::eulerAngleXYZ(0.f, m_transform_handler->rotation.y, 0.f);
	glm::vec3 a1 = displace * glm::vec4(-origin.x, 0, -origin.y, 1);
	glm::vec3 a2 = displace * glm::vec4(size.x - origin.x, 0, -origin.y, 1) ;
	glm::vec3 a3 = displace * glm::vec4(size.x - origin.x, 0, size.y - origin.y, 1) ;
	glm::vec3 a4 = displace * glm::vec4(-origin.x, 0, size.y - origin.y, 1);
	debug_system.draw_line({ {a1, glm::vec4(1)}, {a2, glm::vec4(1)} });
	debug_system.draw_line({ {a2, glm::vec4(1)}, {a3, glm::vec4(1)} });
	debug_system.draw_line({ {a3, glm::vec4(1)}, {a4, glm::vec4(1)} });
	debug_system.draw_line({ {a4, glm::vec4(1)}, {a1, glm::vec4(1)} });
}

void PlaneCollider::ray_intersect(Ray const& ray, rayIntersection& out) const {
	glm::vec3 n(0, 1, 0);
	float t = glm::dot(n, m_transform_handler->position - ray.origin) / glm::dot(n, ray.direction);
	glm::vec3 intersection_point = ray.origin + t * ray.direction;

	glm::vec3 diff = intersection_point - m_transform_handler->position;
	float x_proj = glm::dot(glm::vec3(cos(m_transform_handler->rotation.y), 0, sin(m_transform_handler->rotation.y)), diff);
	float z_proj = glm::dot(glm::vec3(sin(m_transform_handler->rotation.y), 0, cos(m_transform_handler->rotation.y)), diff);

	printf("projects: %f %f\n", x_proj, z_proj);

	if (x_proj < size.x - origin.x && x_proj > -origin.x && z_proj < size.y - origin.y && z_proj > -origin.y) {
		out.points.emplace_back(
			true,
			intersection_point,
			n,
			t
		);
		return;
	}

	return;
}