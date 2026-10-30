#include <LSWE/game/camera.hpp>

#include <cmath>
#include <utility>

namespace LSWE {
namespace Game {

    Camera3D::Camera3D(const Graphics::Display& display_ref, float fov_degrees)
        : m_disp(display_ref)
    {
        set_fov(fov_degrees);
        modify_momentum() = {0.8f, 0.9995f, 0.8f};
        update_projection();
    }

    void Camera3D::set_fov(float fov_degrees) {
        m_last_fov = fov_degrees;

        if (m_last_fov < 10.0f) m_last_fov = 10.0f;
        if (m_last_fov > 150.0f) m_last_fov = 150.0f;

        update_projection();
    }

    void Camera3D::set_max_dist(float dist) {
        m_render_dist = dist;

        if (m_render_dist < 1.0f) m_render_dist = 1.0f;
        if (m_render_dist > 1e6f) m_render_dist = 1e6f;

        update_projection();
    }

    void Camera3D::set_offset_height(float height) {
        m_height = height;
    }

    void Camera3D::set_angle(float yaw, float pitch) {
        m_yaw = yaw;
        m_pitch = pitch;
        validate_angles();
    }

    detail::vector2d Camera3D::get_angle() const {
        return {
            m_yaw,
            m_pitch
        };
    }

    void Camera3D::walk(float side, float forward) {
        const float fx = sin(m_yaw);
        const float fz = -cos(m_yaw);
        const float rx = cos(m_yaw);
        const float rz = sin(m_yaw);

        m_movement.x += (fx * forward + rx * side);
        m_movement.z += (fz * forward + rz * side);
    }

    void Camera3D::angle(float horizontal, float vertical) {
        m_yaw   = fmodf(m_yaw - horizontal, ALLEGRO_PI * 2.0f);
        m_pitch -= vertical;

        validate_angles();
    }
    
    void Camera3D::use() {
        const double cpu_now_time = al_get_time();
        const double alpha = std::min(((cpu_now_time - m_time.last_s) / m_time.delta), 1.0);
        detail::vector3d draw_pos;

        draw_pos.x = alpha * m_position.x + (1.0 - alpha) * m_last_position.x;
        draw_pos.y = alpha * m_position.y + (1.0 - alpha) * m_last_position.y;
        draw_pos.z = alpha * m_position.z + (1.0 - alpha) * m_last_position.z;

        const float fx = sin(m_yaw) * cos(m_pitch);
        const float fy = sin(m_pitch);
        const float fz = -cos(m_yaw) * cos(m_pitch);

        const float eye_x = draw_pos.x;
        const float eye_y = draw_pos.y + m_height;
        const float eye_z = draw_pos.z;

        m_use.build_camera(
            eye_x, eye_y, eye_z,
            eye_x + fx, eye_y + fy, eye_z + fz,
            0.0f, 1.0f, 0.0f
        ).use();
    }

    void Camera3D::use_projection() {
        m_proj_use.use_projection();
    }

    void Camera3D::use_all() {
        use();
        use_projection();
    }

    void Camera3D::update_projection() {
        const float fov_rad = m_last_fov * ALLEGRO_PI / 180.0f;
        const float fov_factor = tan(fov_rad / 2.0f);
        const float aspect = static_cast<float>(m_disp.get_height()) / static_cast<float>(m_disp.get_width());
        
        // Define your desired near plane (e.g., 0.1 units away from the camera)
        const float near_plane = 0.01f; 

        // If your perspective function expects frustum bounds defined AT the near plane, 
        // scale them relative to the near plane distance:
        const float scale = near_plane / (1.0f / fov_factor); 

        m_proj_use.set_identity().perspective(
            -1.0f * scale,       // left
            aspect * scale,      // top
            near_plane,          // near (z-distance) - now much closer!
            1.0f * scale,        // right
            -aspect * scale,     // bottom
            m_render_dist        // far
        );
    }

    void Camera3D::validate_angles() {
        if (m_pitch >  1.4f) m_pitch =  1.4f;
        if (m_pitch < - ALLEGRO_PI / 2.01f) m_pitch = - ALLEGRO_PI / 2.01f;
    }

    void Camera2D::set_limits(float min_x, float min_y, float max_x, float max_y) {
        m_proj_use.set_identity().orthographic(min_x, min_y, -1.0f, max_x, max_y, 1.0f);
    }

    void Camera2D::use() {
        m_use.use();
    }

    void Camera2D::use_projection() {
        m_proj_use.use_projection();
    }

    void Camera2D::use_all() {
        use();
        use_projection();
    }

    
} // namespace Game
} // namespace LSWE