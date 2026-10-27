#pragma once

#include <LSWE/graphics/transform.hpp>
#include <LSWE/graphics/display.hpp>

namespace LSWE {
namespace Game {

    class PlayerCamera {
    public:
        enum class model { M_2D, M_3D };
        enum class axis { X, Y, Z };
        struct axes { float x{}, y{}, z{}; };

        PlayerCamera(model model);

        PlayerCamera& set_limits(axes lim_min, axes lim_max);
        PlayerCamera& set_momentum(axes momentum);
        PlayerCamera& set_gravity(axes gravity);
        PlayerCamera& set_height(float height);

        void teleport(float x, float y);
        void teleport(float x, float y, float z);
        void teleport_angle(float yaw, float pitch);

        float get_x() const;
        float get_y() const;
        float get_z() const;
        
        float get_vx() const;
        float get_vy() const;
        float get_vz() const;

        float get_yaw() const;
        float get_pitch() const;

        float get_tick_time_secs() const;

        void walk_towards(float side, float forward);
        void move_world_pos(float x, float y);
        void move_world_pos(float x, float y, float z);
        void world_block_move_towards(float x, float y);
        void world_block_move_towards(float x, float y, float z);

        void world_block_move_towards_x(float x);
        void world_block_move_towards_y(float y);
        void world_block_move_towards_z(float z);

        void move_angle(float horizontal, float vertical);

        void jump(float how_much, axis axis = axis::Y);

        void cpu_tick();
        const axes& get_world_coords() const;

        void calc_apply_camera();
    private:
        Graphics::Transform m_cached_camera; // use this to build camera on, always

        struct cpu_timings {
            double curr_delta = 0.0;
            double last_time_s = 0.0;

            void tick();
            double get_prop() const;
        };
        
        cpu_timings m_timing;

        // position:
        axes cpu_last, cpu_now;

        // velocity:
        axes m_movement_last, m_movement;

        // acceleration:
        axes m_gravity;

        axes m_momentum;
        axes m_limits[2];

        float m_height = 0.0f; // offset of m_y, which is the person's camera
        float m_yaw = 0.0f;   // Rotation around Y axis
        float m_pitch = 0.0f; // Tilt up/down (guarantee range is within -1.4..1.4)
        
        model m_model;
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