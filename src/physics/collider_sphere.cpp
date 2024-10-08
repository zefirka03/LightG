#include "physics_core.h"

SphereCollider::SphereCollider() {}

boundingBox SphereCollider::get_bounds() const {
    return boundingBox(
		world_transform.position + glm::vec3(-radius),
		world_transform.position + glm::vec3(radius)
	);
}

collisionData SphereCollider::check_collision(Collider* other) {
	if(dynamic_cast<PlaneCollider*>(other))
		return CollisionCheckers::is_collide(this, static_cast<PlaneCollider*>(other));
	else if(dynamic_cast<SpriteCollider*>(other))
		return CollisionCheckers::is_collide(this, static_cast<SpriteCollider*>(other));
	return collisionData();
}

void SphereCollider::update_transform(Transform& transform) {
	world_transform = transform;
}

void SphereCollider::draw_debug(DebugSystem& debug_system) const {
	
}