#include "physics_core.h"

void CollisionCheckers::is_collide(SpriteCollider* a, SpriteCollider* b, collisionData& out) {
	glm::vec3 A_a = a->m_transform_handler->position + glm::vec3(
		(-a->origin.x) * cos(a->m_transform_handler->rotation.y),
		-a->origin.y,
		(-a->origin.x) * sin(a->m_transform_handler->rotation.y)
	);

	glm::vec3 A_b = a->m_transform_handler->position + glm::vec3(
		(a->size.x - a->origin.x) * cos(a->m_transform_handler->rotation.y),
		a->size.y - a->origin.y,
		(a->size.x - a->origin.x) * sin(a->m_transform_handler->rotation.y)
	);

	glm::vec3 B_a = b->m_transform_handler->position + glm::vec3(
		(-b->origin.x) * cos(b->m_transform_handler->rotation.y),
		-b->origin.y,
		(-b->origin.x) * sin(b->m_transform_handler->rotation.y)
	);

	glm::vec3 B_b = b->m_transform_handler->position + glm::vec3(
		(b->size.x - b->origin.x) * cos(b->m_transform_handler->rotation.y),
		b->size.y - b->origin.y,
		(b->size.x - b->origin.x) * sin(b->m_transform_handler->rotation.y)
	);

	boundingBox A(A_a, A_b);
	boundingBox B(B_a, B_b);

	auto intr = is_lines_intersect({glm::vec2(A_a.x, A_a.z), glm::vec2(A_b.x, A_b.z)}, {glm::vec2(B_a.x, B_a.z), glm::vec2(B_b.x, B_b.z)});
	glm::vec2 xz = intr.point;

	float y1 = std::min(A_b.y, B_b.y);
	float y2 = std::max(A_a.y, B_a.y);

	glm::vec3 xyz = { xz.x, (y2 + y1) / 2.f, xz.y };
 
	out.is_collide = A.contains(xyz) && B.contains(xyz);
	out.collision_point = glm::vec3(0);
	out.normal = glm::vec3(0);
	out.distanse = 0;
}

void CollisionCheckers::is_collide(PlaneCollider* a, PlaneCollider* b, collisionData& out) {
	out.is_collide = false;
	out.collision_point = glm::vec3(0);
	out.normal = glm::vec3(0);
	out.distanse = 0;
}

void CollisionCheckers::is_collide(SpriteCollider* a, PlaneCollider* b, collisionData& out) {
	out.is_collide = false;
	out.collision_point = glm::vec3(0);
	out.normal = glm::vec3(0);
	out.distanse = 0;
}

void CollisionCheckers::is_collide(SphereCollider* a, PlaneCollider* b, collisionData& out){
	glm::vec3 a_pos_xz = glm::vec3(a->m_transform_handler->position.x, 0, a->m_transform_handler->position.z);

	glm::vec3 b_a_pos_xz = glm::vec3(b->m_transform_handler->position.x - b->origin.x, 0, b->m_transform_handler->position.z - b->origin.y);
	glm::vec3 b_b_pos_xz = glm::vec3(b->m_transform_handler->position.x - b->origin.x, 0, b->m_transform_handler->position.z - b->origin.y + b->size.y);
	glm::vec3 b_c_pos_xz = glm::vec3(b->m_transform_handler->position.x - b->origin.x + b->size.x, 0, b->m_transform_handler->position.z - b->origin.y + b->size.y);
	glm::vec3 b_d_pos_xz = glm::vec3(b->m_transform_handler->position.x - b->origin.x + b->size.x, 0, b->m_transform_handler->position.z - b->origin.y);

	float y_intersection_vector = a->m_transform_handler->position.y - b->m_transform_handler->position.y;

	bool is_collide = (
		abs(y_intersection_vector) < a->radius &&
		glm::cross(b_b_pos_xz - b_a_pos_xz, a_pos_xz - b_a_pos_xz).y > 0 &&
		glm::cross(b_c_pos_xz - b_b_pos_xz, a_pos_xz - b_b_pos_xz).y > 0 &&
		glm::cross(b_d_pos_xz - b_c_pos_xz, a_pos_xz - b_c_pos_xz).y > 0 &&
		glm::cross(b_a_pos_xz - b_d_pos_xz, a_pos_xz - b_d_pos_xz).y > 0
	);

	out.is_collide = is_collide;
	out.collision_point = glm::vec3(a->m_transform_handler->position.x, b->m_transform_handler->position.y, a->m_transform_handler->position.z);
	out.normal = glm::normalize(glm::vec3(0, y_intersection_vector, 0));
	out.distanse = abs(a->radius - y_intersection_vector);
}

void CollisionCheckers::is_collide(SphereCollider* a, SpriteCollider* b, collisionData& out){
	glm::vec3 b_a_pos_xz = b->m_transform_handler->position + glm::vec3((-b->origin.x) * cos(-b->m_transform_handler->rotation.y), -b->origin.y, (-b->origin.x) * sin(-b->m_transform_handler->rotation.y));
	glm::vec3 b_b_pos_xz = b->m_transform_handler->position + glm::vec3((-b->origin.x + b->size.x) * cos(-b->m_transform_handler->rotation.y), -b->origin.y, (-b->origin.x + b->size.x) * sin(-b->m_transform_handler->rotation.y));
	glm::vec3 b_c_pos_xz = b->m_transform_handler->position + glm::vec3((-b->origin.x + b->size.x) * cos(-b->m_transform_handler->rotation.y), -b->origin.y + b->size.y, (-b->origin.x + b->size.x) * sin(-b->m_transform_handler->rotation.y));
	glm::vec3 b_d_pos_xz = b->m_transform_handler->position + glm::vec3((-b->origin.x) * cos(-b->m_transform_handler->rotation.y), -b->origin.y + b->size.y, (-b->origin.x) * sin(-b->m_transform_handler->rotation.y));

	glm::vec3 l1 = b_b_pos_xz - b_a_pos_xz;
	glm::vec3 l2 = b_c_pos_xz - b_a_pos_xz;

	glm::vec3 n = glm::cross(l1, l2);

	glm::vec3 a_proj = n * (glm::dot(n, b_a_pos_xz - a->m_transform_handler->position)) / glm::dot(n, n) + a->m_transform_handler->position;

	n = glm::normalize(a_proj - a->m_transform_handler->position);
	
	float y_intersection_vector = glm::length(a_proj - a->m_transform_handler->position);

	float A1 = glm::dot(glm::cross(b_b_pos_xz - b_a_pos_xz, a_proj - b_a_pos_xz), n);
	float A2 = glm::dot(glm::cross(b_c_pos_xz - b_b_pos_xz, a_proj - b_b_pos_xz), n);
	float A3 = glm::dot(glm::cross(b_d_pos_xz - b_c_pos_xz, a_proj - b_c_pos_xz), n);
	float A4 = glm::dot(glm::cross(b_a_pos_xz - b_d_pos_xz, a_proj - b_d_pos_xz), n);

	bool is_collide = (
		y_intersection_vector < a->radius && (
			(A1 < 0 && A2 < 0 && A3 < 0 && A4 < 0) ||
			(A1 > 0 && A2 > 0 && A3 > 0 && A4 > 0)
		)
	);

	out.is_collide = is_collide;
	out.collision_point = a_proj;
	out.normal = -n;
	out.distanse = abs(y_intersection_vector - a->radius);
}

void CollisionCheckers::is_collide(SphereCollider* a, SphereCollider* b, collisionData& out) {
	glm::vec3 dist = b->m_transform_handler->position - a->m_transform_handler->position;
	float dist_len = glm::length(dist);
	float distance = a->radius + b->radius - dist_len;

	if (distance > 0) {
		out.is_collide = true;
		out.normal = dist / dist_len;
		out.collision_point = out.normal * a->radius;
		out.distanse = distance;
	} else {
		out.is_collide = false;
		out.distanse = -distance;
	}
}