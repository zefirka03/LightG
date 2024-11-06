#include "rtx_core.h"

RTX_Sprite::RTX_Sprite() {}

RTX_Sprite::RTX_Sprite(
    glm::vec3 _position, 
    glm::vec3 _origin, 
    glm::vec2 _size,
	float _rotation
) : position(_position), origin(_origin), size(_size), rotation(_rotation) {}

void RTX_Sprite::update_bounds() {
    m_bbox = boundingBox(m_transform_handler->position + glm::vec3(
			(-origin.x) * cos(m_transform_handler->rotation.y),
			-origin.y,
			(-origin.x) * sin(m_transform_handler->rotation.y)
		), m_transform_handler->position + glm::vec3(
			(size.x - origin.x) * cos(m_transform_handler->rotation.y),
			size.y - origin.y,
			(size.x - origin.x) * sin(m_transform_handler->rotation.y)
	));
}

void RTX_Sprite::draw_debug(DebugSystem& debug_system) const {}

void RTX_Sprite::pack_data() {
	m_packed_data.object_type = 2;
	memcpy(m_packed_data.packed_data, &position, sizeof(glm::vec3));
	memcpy(m_packed_data.packed_data + 3, &origin, sizeof(glm::vec3));
	memcpy(m_packed_data.packed_data + 6, &size, sizeof(glm::vec2));
	memcpy(m_packed_data.packed_data + 8, &rotation, sizeof(float));
}