#pragma once

#include <variant>
#include <span>

#include <LSWE/game/collisionable_object.hpp>
#include <LSWE/game/drawable_object.hpp>
#include <LSWE/utility/shared_unique_memory.hpp>

namespace LSWE {
namespace Game {

    /**
     * @brief It contains:
     * - VertexBuffer
     * 
     */
    class Entity {
    public:
        using ObjectPtr = std::shared_ptr<Object>;
        using DynamicObjectPtr = std::shared_ptr<DynamicObject>;

        static Entity create_static_entity(std::span<Graphics::Vertex> draw_vertexes, Graphics::Bitmap atlas, ALLEGRO_PRIM_TYPE prim_type = ALLEGRO_PRIM_TRIANGLE_LIST);
        static Entity create_dynamic_entity(std::span<Graphics::Vertex> draw_vertexes, Graphics::Bitmap atlas, ALLEGRO_PRIM_TYPE prim_type = ALLEGRO_PRIM_TRIANGLE_LIST);

        Entity clone_to_static_entity();
        Entity clone_to_dynamic_entity();

        void recreate_vertex_buffer();

        bool is_object() const;
        bool is_dynamic_object() const;

        Object& as_object();
        DynamicObject& as_dynamic_object();
        const Object& as_object() const;
        const DynamicObject& as_dynamic_object() const;

        void tick();
        void draw() const;
    private:
        struct shared_vertex_data {
            struct vertex_reload_data {
                Graphics::Bitmap bitmap;
                std::vector<Graphics::Vertex> vertexes;
                ALLEGRO_PRIM_TYPE prim_type;
            };

            vertex_reload_data reload_data;
            Graphics::VertexBuffer buffer;
            volatile bool must_reload;

            shared_vertex_data(std::span<Graphics::Vertex> draw_vertexes, Graphics::Bitmap atlas, ALLEGRO_PRIM_TYPE prim_type);
            void set_do_reload();
            void check_do_reload();
        };

        Entity(ObjectPtr obj, Utility::SharedUniquePtr<shared_vertex_data> ref);
        Entity(DynamicObjectPtr obj, Utility::SharedUniquePtr<shared_vertex_data> ref);

        std::variant<ObjectPtr, DynamicObjectPtr> m_thing;
        Utility::SharedUniquePtr<shared_vertex_data> m_shared;
    };

} // namespace Game
} // namespace LSWE