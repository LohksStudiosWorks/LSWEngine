#include <LSWE/game/camera.hpp>

#include <cmath>

namespace LSWE {
namespace Game {

    Camera::Camera(const Graphics::Display& display_ref, float fov_degrees)
        : m_disp(display_ref)
    {
        set_fov(fov_degrees);
    }

    void Camera::set_fov(float fov_degrees) {
        m_last_fov = fov_degrees;

        if (m_last_fov < 10.0f) m_last_fov = 10.0f;
        if (m_last_fov > 150.0f) m_last_fov = 150.0f;
    }

    void Camera::set_max_dist(float dist) {
        m_render_dist = dist;

        if (m_render_dist < 1.0f) m_render_dist = 1.0f;
        if (m_render_dist > 1e6f) m_render_dist = 1e6f;
    }

    void Camera::use_projection() {
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