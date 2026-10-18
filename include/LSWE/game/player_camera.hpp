#pragma once

#include <LSWE/graphics/transform.hpp>
#include <LSWE/graphics/display.hpp>

namespace LSWE {
namespace Game {

    class PlayerCamera {
    public:
        PlayerCamera(float min_height);

        void teleport(float x, float y, float z);
        void teleport_angle(float yaw, float pitch);

        float get_x() const;
        float get_y() const;
        float get_z() const;

        void walk_towards(float x, float z);
        void move_world_pos(float x, float y, float z);
        void move_angle(float horizontal, float vertical);

        void jump(float how_much_y);
        void cpu_tick();

        void calc_apply_camera();
    private:
        Graphics::Transform m_cached_camera; // use this to build camera on, always
        float m_height; // offset of m_y, which is the person's camera

        struct position {
            float x, y, z;
        };
        struct cpu_timings {
            double curr_delta = 0.0;
            double last_time_s = 0.0;

            void tick();
            double get_prop() const;
        };
        
        cpu_timings timing;
        position cpu_last, cpu_now;

        float m_gravity_y = 0.0135f;
        float m_resistance_mult_xz = 0.65f;
        float m_min_y = 0.0f;
        float m_min_abs_xz = 0.0001f;


        float m_yaw = 0.0f;   // Rotation around Y axis
        float m_pitch = 0.0f; // Tilt up/down (guarantee range is within -1.4..1.4)
        
        float m_vy = 0.0f;
        float m_vx = 0.0f;
        float m_vz = 0.0f;
    };

    class PlayerPerspective {
    public:
        PlayerPerspective(const Graphics::Display& display_ref, float fov_degrees = 70.0f);

        void set_fov(float fov_degrees);
        void set_max_dist(float dist);
        void build_and_apply();
    private:
        const Graphics::Display& m_disp;
        Graphics::Transform m_perspective;
        float m_last_fov;
        float m_render_dist = 100.0f;
    };
    
} // namespace Game
} // namespace LSWE