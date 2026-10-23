#pragma once

#include <LSWE/game/collisionable_object.hpp>
#include <LSWE/graphics/transform.hpp>
#include <LSWE/graphics/vertex.hpp>

namespace LSWE {
namespace Game {

    class DrawableObject {
    public:
        DrawableObject(const Graphics::VertexArrayType& vertex_ref);

        Graphics::Transform& get_transform();
        Graphics::Transform* operator->();

        void draw();
    protected:
        Graphics::Transform m_object_positioning{};
        const Graphics::VertexArrayType& m_vertex_ref;
    };

    /*class DrawableStaticObject : public DrawableObject {
    
    };

    class DrawableDynamicObject : public DrawableObject {

    };*/

} // namespace Game
} // namespace LSWE