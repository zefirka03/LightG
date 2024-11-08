#pragma once
#include "../physics/quadtree.h"


struct RTX_DrawablePack {
    int32_t object_type;
    int32_t packed_data[15];
};

struct RTX_ReferencePack {
    int child_next;
    int child_prev;
    int node_it;
    int _padding;

    RTX_ReferencePack() {}
    RTX_ReferencePack(
        int _child_next,
        int _child_prev,
        int _node_it
    ) : child_next(_child_next),
        child_prev(_child_prev),
        node_it(_node_it) {}
};

struct RTX_FullChildsPack {
    RTX_ReferencePack reference_pack;
    RTX_DrawablePack drawable_pack;

    RTX_FullChildsPack() {}
    RTX_FullChildsPack(
        RTX_ReferencePack _reference_pack,
        RTX_DrawablePack _drawable_pack
    ) : reference_pack(_reference_pack), 
        drawable_pack(_drawable_pack) {}
};

/*
    m_packed_data is 64 bytes of data, that contains:
    [64 bytes, for full object representation]
*/
class RTX_Drawable : public Quadable {
protected:
friend class RenderRTXSystem;
    Transform* m_transform_handler = nullptr;
    RTX_DrawablePack m_packed_data;

    virtual void draw_debug(DebugSystem& debug_system) const {}
    virtual void pack_data() = 0;
};

class RTX_Sphere : public RTX_Drawable {
public:
   float radius;

   RTX_Sphere();
   RTX_Sphere(float _radius);
   void update_bounds() override;
   void draw_debug(DebugSystem& debug_system) const override;
   void pack_data() override;
};

class RTX_Plane : public RTX_Drawable {
public:
   glm::vec3 position;
   glm::vec3 origin;
   glm::vec2 size;
   float rotation;

   RTX_Plane();
   RTX_Plane(glm::vec3 _position, glm::vec3 _origin, glm::vec2 _size, float _rotation);
   void update_bounds() override;
   void draw_debug(DebugSystem& debug_system) const override;
   void pack_data() override;
};

class RTX_Sprite : public RTX_Drawable {
public:
   glm::vec3 position;
   glm::vec3 origin;
   glm::vec2 size;
   float rotation;

   RTX_Sprite();
   RTX_Sprite(glm::vec3 _position, glm::vec3 _origin, glm::vec2 _size, float _rotation);
   void update_bounds() override;
   void draw_debug(DebugSystem& debug_system) const override;
   void pack_data() override;
};
