#pragma once

#include <tuple>
#include <functional>

#include <LSWE/graphics/vertex.hpp>
#include <LSWE/graphics/transform.hpp>
#include <LSWE/graphics/color.hpp>
#include <LSWE/game/player_camera.hpp>

namespace LSWE {
namespace Game {

    class StaticObject {
    public:
        StaticObject(Graphics::Color color = Graphics::Color(25,125,255));

        StaticObject& make_transform(std::function<void(Graphics::Transform&)> cb);

        void draw_frame();

        bool work_collision(Game::PlayerCamera& player);
    private:
        Graphics::Transform m_boundaries;
        Graphics::Color m_debug_color;
        std::tuple<float,float,float> m_scale_axes{1.0f, 1.0f, 1.0f};

        bool m_test_collided = false;
    };

} // namespace Game
} // namespace LSWE