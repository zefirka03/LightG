#include "physics_core.h"

bool CollisionCheckers::is_collide(SpriteCollider* a, SpriteCollider* b) {
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

	glm::mat2 det = glm::mat2({
		(A_b.z - A_a.z), -(A_b.x - A_a.x),
		(B_b.z - B_a.z), -(B_b.x - B_a.x)
		});

	glm::mat2 x1 = glm::mat2({
		A_a.x * (A_b.z - A_a.z) - A_a.z * (A_b.x - A_a.x), -(A_b.x - A_a.x),
		B_a.x * (B_b.z - B_a.z) - B_a.z * (B_b.x - B_a.x), -(B_b.x - B_a.x)
		});

	glm::mat2 z1 = glm::mat2({
		(A_b.z - A_a.z), A_a.x * (A_b.z - A_a.z) - A_a.z * (A_b.x - A_a.x),
		(B_b.z - B_a.z), B_a.x * (B_b.z - B_a.z) - B_a.z * (B_b.x - B_a.x)
		});

	glm::vec2 xz = { glm::determinant(x1) / glm::determinant(det), glm::determinant(z1) / glm::determinant(det) };

	float y1 = std::min(A_b.y, B_b.y);
	float y2 = std::max(A_a.y, B_a.y);

	glm::vec3 xyz = { xz.x, (y2 + y1) / 2.f, xz.y };

	return A.contains(xyz) && B.contains(xyz);
}

bool CollisionCheckers::is_collide(PlaneCollider* a, PlaneCollider* b) {
	return false;
}

bool CollisionCheckers::is_collide(SpriteCollider* a, PlaneCollider* b) {
	return false;
}