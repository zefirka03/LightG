#include "physics_core.h"

SphereCollider::SphereCollider() {}

void SphereCollider::update_bounds() {
	m_bbox = boundingBox(
		m_transform_handler->position - glm::vec3(radius),
		m_transform_handler->position + glm::vec3(radius)
	);
}

collisionData SphereCollider::check_collision(Collider* other) {
	if(dynamic_cast<PlaneCollider*>(other))
		return CollisionCheckers::is_collide(this, static_cast<PlaneCollider*>(other));
	else if(dynamic_cast<SpriteCollider*>(other))
		return CollisionCheckers::is_collide(this, static_cast<SpriteCollider*>(other));
	return collisionData();
}

void SphereCollider::draw_debug(DebugSystem& debug_system) const {
	
}