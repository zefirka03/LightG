#pragma once
#include "../physics/quadtree.h"

class RTX_Drawable : public Quadable {
protected:
friend class RenderRTXSystem;
    Transform* m_transform_handler = nullptr;

    virtual void draw_debug(DebugSystem& debug_system) const {}
};

class RTX_Sphere : public RTX_Drawable {
public:
   float radius;

   RTX_Sphere();
   RTX_Sphere(float _radius);
   void update_bounds() override;
   void draw_debug(DebugSystem& debug_system) const override;
};
