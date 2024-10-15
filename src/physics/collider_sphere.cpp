#include "physics_core.h"

SphereCollider::SphereCollider() {}

void SphereCollider::update_bounds() {
	m_bbox = boundingBox(
		m_transform_handler->position - glm::vec3(radius),
		m_transform_handler->position + glm::vec3(radius)
	);
}

collisionData SphereCollider::check_collision(Collider* other) {
	if (other->cached_dynamic_cast<PlaneCollider>())
		return CollisionCheckers::is_collide(this, static_cast<PlaneCollider*>(other));
	else if (other->cached_dynamic_cast<SpriteCollider>())
		return CollisionCheckers::is_collide(this, static_cast<SpriteCollider*>(other));
	else if (other->cached_dynamic_cast<SphereCollider>())
		return CollisionCheckers::is_collide(this, static_cast<SphereCollider*>(other));
	return collisionData();
}

void SphereCollider::draw_debug(DebugSystem& debug_system) const {
	
}