#include "physics_core.h"

collisionData CollisionCheckers::is_collide(SpriteCollider* a, SpriteCollider* b) {
	glm::vec3 A_a = a->world_transform.position + glm::vec3(
		(-a->origin.x) * cos(a->world_transform.rotation.y),
		-a->origin.y,
		(-a->origin.x) * sin(a->world_transform.rotation.y)
	);

	glm::vec3 A_b = a->world_transform.position + glm::vec3(
		(a->size.x - a->origin.x) * cos(a->world_transform.rotation.y),
		a->size.y - a->origin.y,
		(a->size.x - a->origin.x) * sin(a->world_transform.rotation.y)
	);

	glm::vec3 B_a = b->world_transform.position + glm::vec3(
		(-b->origin.x) * cos(b->world_transform.rotation.y),
		-b->origin.y,
		(-b->origin.x) * sin(b->world_transform.rotation.y)
	);

	glm::vec3 B_b = b->world_transform.position + glm::vec3(
		(b->size.x - b->origin.x) * cos(b->world_transform.rotation.y),
		b->size.y - b->origin.y,
		(b->size.x - b->origin.x) * sin(b->world_transform.rotation.y)
	);

	boundingBox A(A_a, A_b);
	boundingBox B(B_a, B_b);

	auto intr = is_lines_intersect({glm::vec2(A_a.x, A_a.z), glm::vec2(A_b.x, A_b.z)}, {glm::vec2(B_a.x, B_a.z), glm::vec2(B_b.x, B_b.z)});
	glm::vec2 xz = intr.point;

	float y1 = std::min(A_b.y, B_b.y);
	float y2 = std::max(A_a.y, B_a.y);

	glm::vec3 xyz = { xz.x, (y2 + y1) / 2.f, xz.y };

	return { 
		.is_collide = A.contains(xyz) && B.contains(xyz),
		.collision_point = glm::vec3(0),
		.normal = glm::vec3(0),
		.distanse = 0
	};
}

collisionData CollisionCheckers::is_collide(PlaneCollider* a, PlaneCollider* b) {
	return {
		.is_collide = false,
		.collision_point = glm::vec3(0),
		.normal = glm::vec3(0),
		.distanse = 0
	};
}

collisionData CollisionCheckers::is_collide(SpriteCollider* a, PlaneCollider* b) {
	return {
		.is_collide = false,
		.collision_point = glm::vec3(0),
		.normal = glm::vec3(0),
		.distanse = 0
	};
}

collisionData CollisionCheckers::is_collide(SphereCollider* a, PlaneCollider* b){
	glm::vec3 a_pos_xz = glm::vec3(a->world_transform.position.x, 0, a->world_transform.position.z);

	glm::vec3 b_a_pos_xz = glm::vec3(b->world_transform.position.x - b->origin.x, 0, b->world_transform.position.z - b->origin.y);
	glm::vec3 b_b_pos_xz = glm::vec3(b->world_transform.position.x - b->origin.x, 0, b->world_transform.position.z - b->origin.y + b->size.y);
	glm::vec3 b_c_pos_xz = glm::vec3(b->world_transform.position.x - b->origin.x + b->size.x, 0, b->world_transform.position.z - b->origin.y + b->size.y);
	glm::vec3 b_d_pos_xz = glm::vec3(b->world_transform.position.x - b->origin.x + b->size.x, 0, b->world_transform.position.z - b->origin.y);

	float y_intersection_vector = a->world_transform.position.y - b->world_transform.position.y;

	bool is_collide = (
		abs(y_intersection_vector) < a->radius &&
		glm::cross(b_b_pos_xz - b_a_pos_xz, a_pos_xz - b_a_pos_xz).y > 0 &&
		glm::cross(b_c_pos_xz - b_b_pos_xz, a_pos_xz - b_b_pos_xz).y > 0 &&
		glm::cross(b_d_pos_xz - b_c_pos_xz, a_pos_xz - b_c_pos_xz).y > 0 &&
		glm::cross(b_a_pos_xz - b_d_pos_xz, a_pos_xz - b_d_pos_xz).y > 0
	);

	return {
		.is_collide = is_collide,
		.collision_point = glm::vec3(a->world_transform.position.x, b->world_transform.position.y, a->world_transform.position.z),
		.normal = glm::normalize(glm::vec3(0, y_intersection_vector, 0)),
		.distanse = abs(a->radius - y_intersection_vector)
	};
}

collisionData CollisionCheckers::is_collide(SphereCollider* a, SpriteCollider* b){
	glm::vec3 b_a_pos_xz = b->world_transform.position + glm::vec3((-b->origin.x) * cos(-b->world_transform.rotation.y), -b->origin.y, (-b->origin.x) * sin(-b->world_transform.rotation.y));
	glm::vec3 b_b_pos_xz = b->world_transform.position + glm::vec3((-b->origin.x + b->size.x) * cos(-b->world_transform.rotation.y), -b->origin.y, (-b->origin.x + b->size.x) * sin(-b->world_transform.rotation.y));
	glm::vec3 b_c_pos_xz = b->world_transform.position + glm::vec3((-b->origin.x + b->size.x) * cos(-b->world_transform.rotation.y), -b->origin.y + b->size.y, (-b->origin.x + b->size.x) * sin(-b->world_transform.rotation.y));
	glm::vec3 b_d_pos_xz = b->world_transform.position + glm::vec3((-b->origin.x) * cos(-b->world_transform.rotation.y), -b->origin.y + b->size.y, (-b->origin.x) * sin(-b->world_transform.rotation.y));

	glm::vec3 l1 = b_b_pos_xz - b_a_pos_xz;
	glm::vec3 l2 = b_c_pos_xz - b_a_pos_xz;

	glm::vec3 n = glm::cross(l1, l2);

	glm::vec3 a_proj = n * (glm::dot(n, b_a_pos_xz - a->world_transform.position)) / glm::dot(n, n) + a->world_transform.position;

	n = glm::normalize(a_proj - a->world_transform.position);
	
	float y_intersection_vector = glm::length(a_proj - a->world_transform.position);

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

	return {
		.is_collide = is_collide,
		.collision_point = a_proj,
		.normal = -n,
		.distanse = abs(y_intersection_vector - a->radius)
	};
}