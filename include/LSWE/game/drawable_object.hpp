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


    class DrawableStaticObject : public Drawable {
    public:
        DrawableStaticObject(const Object& ref);

        void draw() const override;
    private:
        const Object& m_ref;
    };

    class DrawableDynamicEntity : public Drawable {
    public:
        DrawableDynamicEntity(const DynamicEntity& ref);

        void draw() const override;
    private:
        const DynamicEntity& m_ref;
    };


    

    class DrawableObject {
    public:
        DrawableObject(const Graphics::VertexArrayType& vertex_ref);

        Graphics::Transform& get_transform();
        Graphics::Transform* operator->();

        void draw() const;
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