#include "physics_core.h"

SpriteCollider::SpriteCollider() {}

boundingBox SpriteCollider::get_bounds() const {
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

bool SpriteCollider::check_collision(Collider* other) {
	if (dynamic_cast<SpriteCollider*>(other))
		return CollisionCheckers::is_collide(this, static_cast<SpriteCollider*>(other));
	else if (dynamic_cast<PlaneCollider*>(other))
		return CollisionCheckers::is_collide(this, static_cast<PlaneCollider*>(other));
	return false;
}

void SpriteCollider::update_transform(Transform& transform) {
	world_transform = transform;
}

void SpriteCollider::draw_debug(DebugSystem& debug_system) const {
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