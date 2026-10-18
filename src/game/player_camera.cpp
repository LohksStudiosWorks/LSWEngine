#include <LSWE/game/player_camera.hpp>

#include <cmath>

namespace LSWE {
namespace Game {

    PlayerCamera::PlayerCamera(float min_height) 
        : m_height(min_height)
    {}

    void PlayerCamera::teleport(float x, float y, float z) {
        cpu_now.x = x;
        cpu_now.y = y; // Ground/feet level
        cpu_now.z = z;
        m_vx = 0.0f;
        m_vy = 0.0f;
        m_vz = 0.0f;
    }

    void PlayerCamera::teleport_angle(float yaw, float pitch) {
        m_yaw = yaw;
        m_pitch = pitch;

        // Clamp pitch to avoid flipping upside down
        if (m_pitch >  1.4f) m_pitch =  1.4f;
        if (m_pitch < -1.4f) m_pitch = -1.4f;
    }

    float PlayerCamera::get_x() const {
        return cpu_now.x;
    }

    float PlayerCamera::get_y() const {
        return cpu_now.y;
    }

    float PlayerCamera::get_z() const {
        return cpu_now.z;
    }

    void PlayerCamera::walk_towards(float x, float z) {
        const float fx = sin(m_yaw);
        const float fz = -cos(m_yaw);
        const float rx = cos(m_yaw);
        const float rz = sin(m_yaw);

        m_vx += (fx * z + rx * x);
        m_vz += (fz * z + rz * x);
    }

    void PlayerCamera::move_world_pos(float x, float y, float z) {
        cpu_now.x += x;
        cpu_now.y += y;
        cpu_now.z += z;
    }

    void PlayerCamera::move_angle(float horizontal, float vertical) {
        constexpr float sensitivity = 0.003f;

        m_yaw -= horizontal * sensitivity;
        m_pitch -= vertical * sensitivity;

        if (m_pitch >  1.4f) m_pitch =  1.4f;
        if (m_pitch < -1.4f) m_pitch = -1.4f;
    }

    void PlayerCamera::jump(float how_much_y) {
        // Only allow jumping if we are on the ground (cpu_now.y <= 0.0f)
        if (cpu_now.y <= 0.0f) {
            m_vy = how_much_y; // Set initial upward velocity impulse
        }
    }

    void PlayerCamera::cpu_tick() {
        timing.tick();

        cpu_last = cpu_now;

        if (cpu_now.y > m_min_y || m_vy > 0.0f) {
            m_vy -= m_gravity_y;
            cpu_now.y += m_vy;
        }

        if (m_vx != 0.0f) {
            cpu_now.x += m_vx;
            m_vx *= m_resistance_mult_xz;
            
            if (fabs(m_vx) < m_min_abs_xz) m_vx = 0;
        }

        if (m_vz != 0.0f) {
            cpu_now.z += m_vz;
            m_vz *= m_resistance_mult_xz;            
            
            if (fabs(m_vz) < m_min_abs_xz) m_vz = 0;
        }

        // Hit the ground check
        if (cpu_now.y <= m_min_y) {
            cpu_now.y = 0.0f;
            m_vy = 0.0f;
        }
    }

    void PlayerCamera::calc_apply_camera() {
        position draw_pos;
        const double alpha = timing.get_prop();

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