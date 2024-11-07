#include "physics_core.h"

SpriteCollider::SpriteCollider() {}

void SpriteCollider::update_bounds() {
    m_bbox = boundingBox(m_transform_handler->position + glm::vec3(
			(-origin.x) * cos(m_transform_handler->rotation.y),
			-origin.y,
			(-origin.x) * sin(m_transform_handler->rotation.y)
		), m_transform_handler->position + glm::vec3(
			(size.x - origin.x) * cos(m_transform_handler->rotation.y),
			size.y - origin.y,
			(size.x - origin.x) * sin(m_transform_handler->rotation.y)
	));
}

void SpriteCollider::check_collision(Collider* other, collisionData& out) {
	if (other->cached_dynamic_cast<SpriteCollider>())
		return CollisionCheckers::is_collide(this, static_cast<SpriteCollider*>(other), out);
	else if (other->cached_dynamic_cast<PlaneCollider>())
		return CollisionCheckers::is_collide(this, static_cast<PlaneCollider*>(other), out);
	else if (other->cached_dynamic_cast<SphereCollider>())
		return CollisionCheckers::is_collide(static_cast<SphereCollider*>(other), this, out);
	return;
}

void SpriteCollider::draw_debug(DebugSystem& debug_system) const {
	glm::mat4 displace = glm::translate(glm::mat4(1), glm::vec3(m_transform_handler->position)) * glm::eulerAngleXYZ(0.f, m_transform_handler->rotation.y, 0.f);
	glm::vec3 a1 = displace * glm::vec4(glm::vec2(0, 0) - origin, 0, 1);
	glm::vec3 a2 = displace * glm::vec4(glm::vec2(size.x, 0) - origin, 0, 1) ;
	glm::vec3 a3 = displace * glm::vec4(glm::vec2(size.x, size.y) - origin, 0, 1) ;
	glm::vec3 a4 = displace * glm::vec4(glm::vec2(0, size.y) - origin, 0, 1);
	debug_system.draw_line({ {a1, glm::vec4(1)}, {a2, glm::vec4(1)} });
	debug_system.draw_line({ {a2, glm::vec4(1)}, {a3, glm::vec4(1)} });
	debug_system.draw_line({ {a3, glm::vec4(1)}, {a4, glm::vec4(1)} });
	debug_system.draw_line({ {a4, glm::vec4(1)}, {a1, glm::vec4(1)} });
}

void SpriteCollider::ray_intersect(Ray const& ray, rayIntersection& out) const {
	glm::vec3 n(sin(m_transform_handler->rotation.y), 0, cos(m_transform_handler->rotation.y));
	float ndot = glm::dot(n, m_transform_handler->position - ray.origin);
	float t = ndot / glm::dot(n, ray.direction);

	if(ndot > 0) n = -n;

	glm::vec3 intersection_point = ray.origin + t * ray.direction;

	if(t > ray.length || t < 0)
        return;

	glm::vec3 diff = intersection_point - m_transform_handler->position;
	float x_proj = glm::dot(glm::cross(n, glm::vec3(0,1,0)), diff);
	float y_proj = glm::dot(glm::vec3(0, 1, 0), diff);

	if(
		x_proj < size.x - origin.x && 
		x_proj > -origin.x &&
		y_proj < size.y - origin.y && 
		y_proj > -origin.y
	){
		out.points.emplace_back(
			true,
			intersection_point,
			n,
			t
		);
	}

	return;
}