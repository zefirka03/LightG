#include "rtx_core.h"

RTX_Sphere::RTX_Sphere() {}

RTX_Sphere::RTX_Sphere(float _radius) : radius(_radius) {}

void RTX_Sphere::update_bounds() {
    m_bbox = boundingBox(
		m_transform_handler->position - glm::vec3(radius),
		m_transform_handler->position + glm::vec3(radius)
	);
}

void RTX_Sphere::draw_debug(DebugSystem& debug_system) const {}

void RTX_Sphere::pack_data() {
	memcpy(m_packed_data.packed_data, &m_transform_handler->position, sizeof(glm::vec3));
	memcpy(m_packed_data.packed_data + sizeof(glm::vec3), &radius, sizeof(float));
}