#include <LSWE/graphics/vertex.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Graphics {

    Vertex::Vertex(float x, float y, ALLEGRO_COLOR c)
        : ALLEGRO_VERTEX({x, y, 0, 0, 0, c})
    {}

    Vertex::Vertex(float x, float y, float z, ALLEGRO_COLOR c)
        : ALLEGRO_VERTEX({x, y, z, 0, 0, c})
    {}

    Vertex::Vertex(float x, float y, float z, float u, float v, ALLEGRO_COLOR c)
        : ALLEGRO_VERTEX({x, y, z, u, v, c})
    {}

    Vertexes Vertexes::create(std::span<Vertex> vertexes, type type, int flags, std::optional<Bitmap> texture) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();

        const ALLEGRO_VERTEX* vertex_data = vertexes.data();
        
        return Vertexes(generate_gpu_buffer(            
            vertex_data,
            vertexes.size(),
            flags
        ), vertexes.size(), type, std::move(texture));
    }

    void Vertexes::replace(std::span<Vertex> vertexes, type type, int flags) {
        m_gpu_vertexes.reset(); // free up first
        m_gpu_vertexes = std::shared_ptr<ALLEGRO_VERTEX_BUFFER>(generate_gpu_buffer(            
            vertexes.data(),
            vertexes.size(),
            flags
        ), al_destroy_vertex_buffer);
        m_type = type;
        m_vertex_count = vertexes.size();

        if (!m_gpu_vertexes)
            throw Exception::NullException("Vertexes was null");
    }

    void Vertexes::set_texture(Bitmap bitmap) {
        m_atlas_texture.reset();
        m_atlas_texture = bitmap;
    }

    void Vertexes::remove_texture() {
        m_atlas_texture.reset();
    }

    size_t Vertexes::size() const {
        return m_vertex_count;
    }

    void Vertexes::draw() const {
        Transform bak(al_get_current_transform());
        Transform combine = m_object_positioning.clone();
        combine.compose(bak);

        combine.use();

        al_draw_vertex_buffer(
            m_gpu_vertexes.get(),
            m_atlas_texture.has_value() ? m_atlas_texture.value() : nullptr,
            0, static_cast<int>(m_vertex_count),
            static_cast<int>(m_type)
        );
        
        bak.use();
    }
    
    void Vertexes::draw(size_t from, size_t to) {
        if (from >= to || to > m_vertex_count) return;

        Transform bak(al_get_current_transform());
        Transform combine = m_object_positioning.clone();
        combine.compose(bak);

        combine.use();
        al_draw_vertex_buffer(
            m_gpu_vertexes.get(),
            m_atlas_texture.has_value() ? m_atlas_texture.value() : nullptr,
            static_cast<int>(from), static_cast<int>(to),
            static_cast<int>(m_type)
        );
        
        bak.use();
    }

    void Vertexes::draw(size_t from, size_t to, Bitmap with_texture) {
        if (from >= to || to > m_vertex_count) return;
        
        Transform bak(al_get_current_transform());
        Transform combine = m_object_positioning.clone();
        combine.compose(bak);

        combine.use();
        al_draw_vertex_buffer(
            m_gpu_vertexes.get(),
            with_texture,
            static_cast<int>(from), static_cast<int>(to),
            static_cast<int>(m_type)
        );

        bak.use();
    }

    Transform& Vertexes::get_transform() {
        return m_object_positioning;
    }

    Transform* Vertexes::operator->() {
        return &m_object_positioning;
    }

    Vertexes::Vertexes(ALLEGRO_VERTEX_BUFFER* gpu_vertexes, size_t len, type type, std::optional<Bitmap> bitmap)
        : m_gpu_vertexes(gpu_vertexes, al_destroy_vertex_buffer),
          m_atlas_texture(std::move(bitmap)),
          m_vertex_count(len),
          m_type(type)
    {
        if (!m_gpu_vertexes)
            throw Exception::NullException("Vertexes was null");
    }

    ALLEGRO_VERTEX_BUFFER* Vertexes::generate_gpu_buffer(const ALLEGRO_VERTEX* data, size_t len, int flags) {
        return al_create_vertex_buffer(
            nullptr,
            data,
            len,
            flags
        );   
    }
} // namespace Graphics
} // namespace LSWE