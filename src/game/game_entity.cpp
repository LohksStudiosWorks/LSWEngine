#include <LSWE/game/game_entity.hpp>

#include <utility>

namespace LSWE {
namespace Game {

    Entity Entity::create_static_entity(std::span<Graphics::Vertex> draw_vertexes, Graphics::Bitmap atlas, ALLEGRO_PRIM_TYPE prim_type) {
        return Entity(std::make_shared<Object>(), new shared_vertex_data(
            draw_vertexes,
            atlas,
            prim_type)
        );
    }

    Entity Entity::create_dynamic_entity(std::span<Graphics::Vertex> draw_vertexes, Graphics::Bitmap atlas, ALLEGRO_PRIM_TYPE prim_type) {
        return Entity(std::make_shared<DynamicObject>(), new shared_vertex_data(
            draw_vertexes,
            atlas,
            prim_type)
        );
    }

    Entity Entity::clone_to_static_entity() {
        return Entity(std::make_shared<Object>(), m_shared);
    }

    Entity Entity::clone_to_dynamic_entity() {
        return Entity(std::make_shared<DynamicObject>(), m_shared);
    }

    void Entity::recreate_vertex_buffer() {
        m_shared->set_do_reload();
    }

    bool Entity::is_object() const {
        return std::holds_alternative<ObjectPtr>(m_thing);
    }

    bool Entity::is_dynamic_object() const {
        return std::holds_alternative<DynamicObjectPtr>(m_thing);
    }

    Object& Entity::as_object() {
        return *std::get<ObjectPtr>(m_thing);
    }

    DynamicObject& Entity::as_dynamic_object() {
        return *std::get<DynamicObjectPtr>(m_thing);
    }

    const Object& Entity::as_object() const {
        return *std::get<ObjectPtr>(m_thing);
    }

    const DynamicObject& Entity::as_dynamic_object() const {
        return *std::get<DynamicObjectPtr>(m_thing);
    }

    void Entity::tick() {
        if (is_dynamic_object()) {
            as_dynamic_object().tick();
        }
    }

    void Entity::draw() const {
        m_shared->check_do_reload();
        Graphics::Transform bak(al_get_current_transform());

        if (is_object()) {
            auto& obj = as_object();

            Graphics::Transform combine = obj.get_draw_boundaries().clone();
            combine.compose(bak);

            combine.use();

            m_shared->buffer.draw();
            
            bak.use();
        }
        else {
            auto& obj = as_dynamic_object();

            Graphics::Transform combine = obj.get_draw_boundaries().clone();
            combine.compose(bak);

            combine.use();

            m_shared->buffer.draw();
            
            bak.use();
        }
    }

    Entity::shared_vertex_data::shared_vertex_data(std::span<Graphics::Vertex> draw_vertexes, Graphics::Bitmap atlas, ALLEGRO_PRIM_TYPE prim_type)
        : reload_data({
            atlas,
            std::vector<Graphics::Vertex>(draw_vertexes.begin(), draw_vertexes.end()),
            prim_type
        }),
        buffer(
            draw_vertexes,
            prim_type,
            0,
            atlas
        ),
        must_reload(false)
    {}
    
    void Entity::shared_vertex_data::set_do_reload() {
        must_reload = true;
    }

    void Entity::shared_vertex_data::check_do_reload() {
        if (std::exchange(must_reload, false)) {
            buffer.recreate(reload_data.vertexes, reload_data.prim_type, 0, reload_data.bitmap);
        }
    }

    Entity::Entity(ObjectPtr obj, Utility::SharedUniquePtr<shared_vertex_data> ref)
        : m_thing(obj),
          m_shared(ref)
    {}

    Entity::Entity(DynamicObjectPtr obj, Utility::SharedUniquePtr<shared_vertex_data> ref)
        : m_thing(obj),
          m_shared(ref)
    {}



} // namespace Game
} // namespace LSWE