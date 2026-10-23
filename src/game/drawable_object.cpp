#include <LSWE/game/drawable_object.hpp>

namespace LSWE {
namespace Game {

    DrawableObject::DrawableObject(const Graphics::VertexArrayType& vertex_ref)
        : m_vertex_ref(vertex_ref)
    {}

    Graphics::Transform& DrawableObject::get_transform() {
        return m_object_positioning;
    }

    Graphics::Transform* DrawableObject::operator->() {
        return &m_object_positioning;
    }

    void DrawableObject::draw() {
        Graphics::Transform bak(al_get_current_transform());
        Graphics::Transform combine = m_object_positioning.clone();
        combine.compose(bak);

        combine.use();

        m_vertex_ref.draw();
        
        bak.use();
    }

} // namespace Game
} // namespace LSWE