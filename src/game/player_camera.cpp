#include <LSWE/game/player_camera.hpp>

#include <cmath>

namespace LSWE {
namespace Game {

    PlayerCamera::PlayerCamera(model model) 
        : m_model(model)
    {}

    PlayerCamera& PlayerCamera::set_limits(axes lim_min, axes lim_max) {
        m_limits[0] = lim_min;
        m_limits[1] = lim_max;
        return *this;
    }

    PlayerCamera& PlayerCamera::set_momentum(axes momentum) {
        m_momentum = momentum;
        return *this;
    }

    PlayerCamera& PlayerCamera::set_gravity(axes gravity) {
        m_gravity = gravity;
        return *this;
    }

    PlayerCamera& PlayerCamera::set_height(float height) {
        m_height = height;
        return *this;
    }

    void PlayerCamera::teleport(float x, float y) {
        cpu_now.x = x;
        cpu_now.y = y;
        cpu_now.z = 0;
        m_movement.x = 0.0f;
        m_movement.y = 0.0f;
    }

    void PlayerCamera::teleport(float x, float y, float z) {
        cpu_now.x = x;
        cpu_now.y = y; // Ground/feet level
        cpu_now.z = z;
        m_movement.x = 0.0f;
        m_movement.y = 0.0f;
        m_movement.z = 0.0f;
    }

    void PlayerCamera::teleport_angle(float yaw, float pitch) {
        m_yaw = yaw;
        m_pitch = pitch;

        // Clamp pitch to avoid flipping upside down
        if (m_pitch >  1.4f) m_pitch =  1.4f;
        if (m_pitch < -1.4f) m_pitch = -1.4f;
    }

    float PlayerCamera::get_x() const {
        return cpu_last.x;
    }

    float PlayerCamera::get_y() const {
        return cpu_last.y;
    }

    float PlayerCamera::get_z() const {
        return cpu_last.z;
    }

    float PlayerCamera::get_vx() const {
        return m_movement_last.x;
    }

    float PlayerCamera::get_vy() const {
        return m_movement_last.y;
    }

    float PlayerCamera::get_vz() const {
        return m_movement_last.z;
    }

    float PlayerCamera::get_yaw() const {
        return m_yaw;
    }

    float PlayerCamera::get_pitch() const {
        return m_pitch;
    }
    
    float PlayerCamera::get_tick_time_secs() const {
        return static_cast<float>(1.0 / std::max(m_timing.curr_delta, 1e-20));
    }

    void PlayerCamera::walk_towards(float side, float forward) {
        switch(m_model) {
        case model::M_2D:
        {
            m_movement.x += side;
            m_movement.y += forward;
        }
            break;
        case model::M_3D:
        {
            const float fx = sin(m_yaw);
            const float fz = -cos(m_yaw);
            const float rx = cos(m_yaw);
            const float rz = sin(m_yaw);

            m_movement.x += (fx * forward + rx * side);
            m_movement.z += (fz * forward + rz * side);
        }
            break;
        }
    }

    void PlayerCamera::move_world_pos(float x, float y) {
        cpu_now.x += x;
        cpu_now.y += y;
    }

    void PlayerCamera::move_world_pos(float x, float y, float z) {
        cpu_now.x += x;
        cpu_now.y += y;
        cpu_now.z += z;
    }

    void PlayerCamera::world_block_move_towards(float x, float y) {
        cpu_now.x += x;
        cpu_now.y += y;
        if (m_movement.x * x < 0.0f) m_movement.x = 0.0f;
        if (m_movement.y * y < 0.0f) m_movement.y = 0.0f;
    }

    void PlayerCamera::world_block_move_towards(float x, float y, float z) {
        cpu_now.x += x;
        cpu_now.y += y;
        cpu_now.z += z;
        if (m_movement.x * x < 0.0f) m_movement.x = 0.0f;
        if (m_movement.y * y < 0.0f) m_movement.y = 0.0f;
        if (m_movement.z * z < 0.0f) m_movement.z = 0.0f;
    }

    void PlayerCamera::world_block_move_towards_x(float x) {
        cpu_now.x += x;
        if (m_movement.x * x < 0.0f) m_movement.x = 0.0f;
    }

    void PlayerCamera::world_block_move_towards_y(float y) {
        cpu_now.y += y;
        if (m_movement.y * y < 0.0f) m_movement.y = 0.0f;
    }

    void PlayerCamera::world_block_move_towards_z(float z) {
        cpu_now.z += z;
        if (m_movement.z * z < 0.0f) m_movement.z = 0.0f;
    }    

    void PlayerCamera::move_angle(float horizontal, float vertical) {
        constexpr float sensitivity = 0.003f;

        m_yaw -= horizontal * sensitivity;
        m_pitch -= vertical * sensitivity;

        if (m_pitch >  1.4f) m_pitch =  1.4f;
        if (m_pitch < - ALLEGRO_PI / 2.01f) m_pitch = - ALLEGRO_PI / 2.01f;
    }

    void PlayerCamera::jump(float how_much, axis axis) {
        switch(axis) {
        case axis::X:
            m_movement.x = how_much;
            break;
        case axis::Y:
            m_movement.y = how_much;
            break;
        case axis::Z:
            m_movement.z = how_much;
            break;
        }
    }

    void PlayerCamera::cpu_tick() {
        axes calc{cpu_now};

        m_movement.x = m_movement.x * m_momentum.x + m_gravity.x;
        m_movement.y = m_movement.y * m_momentum.y + m_gravity.y;
        m_movement.z = m_movement.z * m_momentum.z + m_gravity.z;

        calc.x += m_movement.x;
        calc.y += m_movement.y;
        calc.z += m_movement.z;        

        if (calc.x < m_limits[0].x) { 
            calc.x = m_limits[0].x;
            if (m_movement.x < 0.0f) m_movement.x = 0.0f;
        }
        if (calc.y < m_limits[0].y) { 
            calc.y = m_limits[0].y;
            if (m_movement.y < 0.0f) m_movement.y = 0.0f;
        }
        if (calc.z < m_limits[0].z) { 
            calc.z = m_limits[0].z;
            if (m_movement.z < 0.0f) m_movement.z = 0.0f;
        }

        if (calc.x > m_limits[1].x) { 
            calc.x = m_limits[1].x;
            if (m_movement.x > 0.0f) m_movement.x = 0.0f;
        }
        if (calc.y > m_limits[1].y) { 
            calc.y = m_limits[1].y;
            if (m_movement.y > 0.0f) m_movement.y = 0.0f;
        }
        if (calc.z > m_limits[1].z) { 
            calc.z = m_limits[1].z;
            if (m_movement.z > 0.0f) m_movement.z = 0.0f;
        }

        m_timing.tick();
        cpu_last = cpu_now;
        cpu_now = calc;
        m_movement_last = m_movement;
    }

    void PlayerCamera::calc_apply_camera() {
        axes draw_pos;
        const double alpha = m_timing.get_prop();

        draw_pos.x = alpha * cpu_now.x + (1.0 - alpha) * cpu_last.x;
        draw_pos.y = alpha * cpu_now.y + (1.0 - alpha) * cpu_last.y;
        draw_pos.z = alpha * cpu_now.z + (1.0 - alpha) * cpu_last.z;

        const float fx = sin(m_yaw) * cos(m_pitch);
        const float fy = sin(m_pitch);
        const float fz = -cos(m_yaw) * cos(m_pitch);

        const float eye_x = draw_pos.x;
        const float eye_y = draw_pos.y + m_height;
        const float eye_z = draw_pos.z;

        m_cached_camera.build_camera(
            eye_x, eye_y, eye_z,
            eye_x + fx, eye_y + fy, eye_z + fz,
            0.0f, 1.0f, 0.0f
        ).use();
    }

    void PlayerCamera::cpu_timings::tick() {
        const double cpu_now_time = al_get_time();
        curr_delta = cpu_now_time - last_time_s;
        last_time_s = cpu_now_time;
    }

    double PlayerCamera::cpu_timings::get_prop() const {
        const double cpu_now_time = al_get_time();
        return std::min(((cpu_now_time - last_time_s) / curr_delta), 1.0);
    }

    PlayerPerspective::PlayerPerspective(const Graphics::Display& display_ref, float fov_degrees)
        : m_disp(display_ref)
    {
        set_fov(fov_degrees);
    }

    void PlayerPerspective::set_fov(float fov_degrees) {
        m_last_fov = fov_degrees;

        if (m_last_fov < 10.0f) m_last_fov = 10.0f;
        if (m_last_fov > 150.0f) m_last_fov = 150.0f;
    }

    void PlayerPerspective::set_max_dist(float dist) {
        m_render_dist = dist;

        if (m_render_dist < 1.0f) m_render_dist = 1.0f;
        if (m_render_dist > 1e6f) m_render_dist = 1e6f;
    }

    void PlayerPerspective::build_and_apply() {
        const float fov_rad = m_last_fov * ALLEGRO_PI / 180.0f;
        const float fov_factor = tan(fov_rad / 2.0f);
        const float aspect = static_cast<float>(m_disp.get_height()) / static_cast<float>(m_disp.get_width());

        m_perspective.set_identity().perspective(
            -1.0f,               // left
            aspect,              // top
            1.0f / fov_factor,   // near (z-distance)
            1.0f,                // right
            -aspect,             // bottom
            m_render_dist        // far
        ).use_projection();
    }
    
    
} // namespace Game
} // namespace LSWE