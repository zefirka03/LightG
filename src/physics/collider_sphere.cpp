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

void SphereCollider::ray_intersect(Ray const& ray, rayIntersection& out) const {
	float t0, t1;

    glm::vec3 L = m_transform_handler->position - ray.origin;
    float tca = glm::dot(L, ray.direction);
    // if (tca < 0) return false;
    float d2 = glm::dot(L, L) - tca * tca;
    if (d2 > radius * radius) {
		out.is_intersect = false;
		return;
	}

    float thc = sqrt(radius * radius - d2);
    t0 = tca - thc;
    t1 = tca + thc;

	if (t0 > t1) std::swap(t0, t1);

    if (t0 < 0) {
    	t0 = t1; // If t0 is negative, let's use t1 instead.
        if (t0 < 0) {
			out.is_intersect = false;
			return;
		}
    }
	if(t0 < ray.length) 
		out.points.emplace_back(ray.origin + ray.direction * t0);
	if (t1 < ray.length) 
		out.points.emplace_back(ray.origin + ray.direction * t1);

	out.is_intersect = !out.points.empty();
    return;
}