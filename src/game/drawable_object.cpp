#include <LSWE/game/drawable_object.hpp>

namespace LSWE {
namespace Game {

    namespace detail {
        struct debug_box {
            ALLEGRO_VERTEX verts[24];
            debug_box();

            void change_color(ALLEGRO_COLOR color = al_map_rgb(25,125,255));
        };

        debug_box::debug_box() {
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

        void debug_box::change_color(ALLEGRO_COLOR color) {
            for(auto& i : verts) i.color = color;
        }

        static debug_box box;
    }

    Drawable& Drawable::set_color(Graphics::Color color) {
        m_color = color;
        return *this;
    }

    Drawable& Drawable::set_hit_color(Graphics::Color color) {
        m_hit_color = color;
        return *this;
    }




    WrapObjectDrawable::WrapObjectDrawable(const Object& ref)
        : m_ref(ref)
    {}

    void WrapObjectDrawable::draw() const {

        Graphics::Transform bak(al_get_current_transform());
        Graphics::Transform combine = m_ref.get_draw_boundaries().clone();
        combine.compose(bak);

        combine.use();

        detail::box.change_color(m_ref.m_test_collided ? m_hit_color : m_color);
        al_draw_prim(detail::box.verts, NULL, NULL, 0, 24, ALLEGRO_PRIM_LINE_LIST);
        
        bak.use();
    }


    WrapDynamicPointDrawable::WrapDynamicPointDrawable(const DynamicPoint& ref)
        : m_ref(ref)
    {}

    void WrapDynamicPointDrawable::draw() const {

        Graphics::Transform bak(al_get_current_transform());
        Graphics::Transform combine;
        const auto& pos = m_ref.get_position();

        combine
            .scale(0.15f, 0.15f, 0.15f)
            .translate(pos.x, pos.y, pos.z);
        combine.compose(bak);

        combine.use();

        detail::box.change_color(/*m_ref.m_test_collided ? m_hit_color :*/ m_color);
        al_draw_prim(detail::box.verts, NULL, NULL, 0, 24, ALLEGRO_PRIM_LINE_LIST);
        
        bak.use();
    }









    WrapVertexDrawable::WrapVertexDrawable(const Graphics::VertexArrayType& vertex_ref)
        : m_vertex_ref(vertex_ref)
    {}

    Graphics::Transform& WrapVertexDrawable::get_transform() {
        return m_object_positioning;
    }

    Graphics::Transform* WrapVertexDrawable::operator->() {
        return &m_object_positioning;
    }

    void WrapVertexDrawable::draw() const {
        Graphics::Transform bak(al_get_current_transform());
        Graphics::Transform combine = m_object_positioning.clone();
        combine.compose(bak);

        combine.use();

        m_vertex_ref.draw();
        
        bak.use();
    }

} // namespace Game
} // namespace LSWE