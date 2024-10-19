#include "physics_core.h"

SphereCollider::SphereCollider() {}

void SphereCollider::update_bounds() {
	m_bbox = boundingBox(
		m_transform_handler->position - glm::vec3(radius),
		m_transform_handler->position + glm::vec3(radius)
	);
}


void SphereCollider::check_collision(Collider* other, collisionData& out) {
	if (other->cached_dynamic_cast<PlaneCollider>())
		return CollisionCheckers::is_collide(this, static_cast<PlaneCollider*>(other), out);
	else if (other->cached_dynamic_cast<SpriteCollider>())
		return CollisionCheckers::is_collide(this, static_cast<SpriteCollider*>(other), out);
	else if (other->cached_dynamic_cast<SphereCollider>())
		return CollisionCheckers::is_collide(this, static_cast<SphereCollider*>(other), out);
	return;
}

void SphereCollider::draw_debug(DebugSystem& debug_system) const {
	
}