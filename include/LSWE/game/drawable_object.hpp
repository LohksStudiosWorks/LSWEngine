#pragma once

#include <LSWE/game/collisionable_object.hpp>
#include <LSWE/graphics/transform.hpp>
#include <LSWE/graphics/vertex.hpp>

namespace LSWE {
namespace Game {

    class Drawable {
    public:
        Drawable& set_color(Graphics::Color color);
        Drawable& set_hit_color(Graphics::Color color);

        virtual void draw() const = 0;
    protected:
        Graphics::Color m_color{25,255,125}, m_hit_color{255,0,0};
    };


    class WrapObjectDrawable : public Drawable {
    public:
        WrapObjectDrawable(const Object& ref);

        void draw() const override;
    private:
        const Object& m_ref;
    };

    class WrapDynamicEntityDrawable : public Drawable {
    public:
        WrapDynamicEntityDrawable(const DynamicPoint& ref);

        void draw() const override;
    private:
        const DynamicPoint& m_ref;
    };
  
    class WrapVertexDrawable {
    public:
        WrapVertexDrawable(const Graphics::VertexArrayType& vertex_ref);

        Graphics::Transform& get_transform();
        Graphics::Transform* operator->();

        void draw() const;
    protected:
        Graphics::Transform m_object_positioning{};
        const Graphics::VertexArrayType& m_vertex_ref;
    };

    /*class WrapObjectDrawable : public WrapVertexDrawable {
    
    };

    class DrawableDynamicObject : public WrapVertexDrawable {

    };*/

} // namespace Game
} // namespace LSWE