#pragma once

#include <LSWE/graphics/transform.hpp>
#include <LSWE/graphics/display.hpp>

#include <LSWE/game/collisionable_object.hpp>

namespace LSWE {
namespace Game {

    /**
     * @brief This is an interface to Projection transformation that is also
     * collide-able with other entities / objects.
     */
    class Camera : public DynamicEntity {
    public:
        Camera(const Graphics::Display& display_ref, float fov_degrees = 90.0f);

        void set_fov(float fov_degrees);
        void set_max_dist(float dist);
        void set_offset_height(float height);
        void set_angle(float yaw, float pitch);

        void walk(float side, float forward);
        void angle(float horizontal, float vertical);

        void set_update_projection();

        void use();
        void use_projection();
    private:
        void update_projection();

        void validate_angles();
       
        Graphics::Transform m_use, m_proj_use;

        const Graphics::Display& m_disp;
        float m_last_fov;
        float m_render_dist = 100.0f;

        float m_yaw = 0.0f;   // Rotation around Y axis
        float m_pitch = 0.0f; // Tilt up/down (guarantee range is within -1.4..1.4)
        float m_height = 2.0f;

        bool m_update_proj = true;
    };
    
} // namespace Game
} // namespace LSWE