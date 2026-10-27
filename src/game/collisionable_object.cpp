#include <LSWE/game/collisionable_object.hpp>

#include <cmath>

namespace LSWE {
namespace Game {

    namespace detail {
        struct box_debug {
            ALLEGRO_VERTEX verts[24];
            box_debug();

            void change_color(ALLEGRO_COLOR color = al_map_rgb(25,125,255));
        };

        box_debug::box_debug() {
            // Helper lambda or inline coordinates for the 12 edges of a cube (24 vertices for ALLEGRO_PRIM_LINE_LIST)
            const ALLEGRO_COLOR color = al_map_rgb(25,125,255);
            constexpr float x0 = 0.0f, x1 = 1.0f;
            constexpr float y0 = 0.0f, y1 = 1.0f;
            constexpr float z0 = 0.0f, z1 = 1.0f;

            // Define vertices for the 12 edges:
            // Bottom square
            verts[0]  = {x0, y0, z0, 0, 0, color}; verts[1]  = {x1, y0, z0, 0, 0, color};
            verts[2]  = {x1, y0, z0, 0, 0, color}; verts[3]  = {x1, y0, z1, 0, 0, color};
            verts[4]  = {x1, y0, z1, 0, 0, color}; verts[5]  = {x0, y0, z1, 0, 0, color};
            verts[6]  = {x0, y0, z1, 0, 0, color}; verts[7]  = {x0, y0, z0, 0, 0, color};

            // Top square
            verts[8]  = {x0, y1, z0, 0, 0, color}; verts[9]  = {x1, y1, z0, 0, 0, color};
            verts[10] = {x1, y1, z0, 0, 0, color}; verts[11] = {x1, y1, z1, 0, 0, color};
            verts[12] = {x1, y1, z1, 0, 0, color}; verts[13] = {x0, y1, z1, 0, 0, color};
            verts[14] = {x0, y1, z1, 0, 0, color}; verts[15] = {x0, y1, z0, 0, 0, color};

            // Connecting vertical pillars
            verts[16] = {x0, y0, z0, 0, 0, color}; verts[17] = {x0, y1, z0, 0, 0, color};
            verts[18] = {x1, y0, z0, 0, 0, color}; verts[19] = {x1, y1, z0, 0, 0, color};
            verts[20] = {x1, y0, z1, 0, 0, color}; verts[21] = {x1, y1, z1, 0, 0, color};
            verts[22] = {x0, y0, z1, 0, 0, color}; verts[23] = {x0, y1, z1, 0, 0, color};
        }

        void box_debug::change_color(ALLEGRO_COLOR color) {
            for(auto& i : verts) i.color = color;
        }

        static box_debug box;
    }
    
    StaticObject::StaticObject(Graphics::Color color)
        : m_debug_color(color)
    {}

    StaticObject& StaticObject::make_transform(std::function<void(Graphics::Transform&)> cb) {
        if (cb) cb(m_boundaries);
        float x[2]{0.0f, 1.0f}, y[2]{0.0f, 1.0f}, z[2]{0.0f, 1.0f};
        
        m_boundaries.transform_coordinates(x[0],y[0],z[0]);
        m_boundaries.transform_coordinates(x[1],y[1],z[1]);

        m_scale_axes = {
            fabs(x[1]-x[0]),
            fabs(y[1]-y[0]),
            fabs(z[1]-z[0])
        };

        return *this;
    }

    void StaticObject::draw_frame() {

        Graphics::Transform bak(al_get_current_transform());
        Graphics::Transform combine = m_boundaries.clone();
        combine.compose(bak);

        combine.use();

        detail::box.change_color(m_test_collided ? al_map_rgb(255,0,0) : m_debug_color);
        al_draw_prim(detail::box.verts, NULL, NULL, 0, 24, ALLEGRO_PRIM_LINE_LIST);
        
        bak.use();
    }

    bool StaticObject::work_collision(Game::PlayerCamera& player) {
        float tx = 1.0f, ty = 1.0f, tz = 1.0f;
        Graphics::Transform inverse = m_boundaries.clone().invert_3d();

        float x = player.get_x();
        float y = player.get_y();
        float z = player.get_z();
        
        inverse.transform_coordinates(x, y, z);

        if (m_test_collided = (x >= 0.0f && x <= 1.0f &&
            y >= 0.0f && y <= 1.0f &&
            z >= 0.0f && z <= 1.0f))
        {
            constexpr float off = 1.0001f;
            const float mx = (x < 0.5f ? - x : 1.0f - x);
            const float my = (y < 0.5f ? - y : 1.0f - y);
            const float mz = (z < 0.5f ? - z : 1.0f - z);
            const auto& [sx, sy, sz] = m_scale_axes;

            if (my != 0.0f && fabs(my) < 0.2f) {
                player.world_block_move_towards_y(my * sy * off);
            }
            else if (mx != 0.0f && fabs(mx) * sx < fabs(mz) * sz) {
                player.world_block_move_towards_x(mx * sx * off);
            }
            else {
                player.world_block_move_towards_z(mz * sz * off);
            }
            return true;
        }
        return false;
    }

} // namespace Game
} // namespace LSWE