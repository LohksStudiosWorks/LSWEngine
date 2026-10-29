#pragma once

#include <LSWE/graphics/transform.hpp>
#include <LSWE/graphics/display.hpp>

namespace LSWE {
namespace Game {

    /**
     * @brief This is an interface to Projection transformation.
     */
    class Camera {
    public:
        Camera(const Graphics::Display& display_ref, float fov_degrees = 70.0f);

        void set_fov(float fov_degrees);
        void set_max_dist(float dist);

        void use_projection();
    private:
        const Graphics::Display& m_disp;
        Graphics::Transform m_perspective;
        float m_last_fov;
        float m_render_dist = 100.0f;
    };
    
} // namespace Game
} // namespace LSWE