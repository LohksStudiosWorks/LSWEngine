#include <LSWE/graphics/vertex.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Graphics {

    Vertex::Vertex()
        : ALLEGRO_VERTEX({0, 0, 0, 0, 0, al_map_rgb(255,255,255)}) 
    {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
    }

    Vertex::Vertex(float x, float y, ALLEGRO_COLOR c)
        : ALLEGRO_VERTEX({x, y, 0, 0, 0, c})
    {}

    Vertex::Vertex(float x, float y, float z, ALLEGRO_COLOR c)
        : ALLEGRO_VERTEX({x, y, z, 0, 0, c})
    {}

    Vertex::Vertex(float x, float y, float z, float u, float v, ALLEGRO_COLOR c)
        : ALLEGRO_VERTEX({x, y, z, u, v, c})
    {}
    
    // VertexDecl:

    VertexDecl::VertexDecl(const ALLEGRO_VERTEX_ELEMENT* elements, int size_stride) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();

        m_decl = std::shared_ptr<ALLEGRO_VERTEX_DECL>(
            al_create_vertex_decl(elements, size_stride),
            al_destroy_vertex_decl
        );
    }

    VertexDecl::operator ALLEGRO_VERTEX_DECL*() const {
        return m_decl.get();
    }

    // IndexBuffer:

    IndexBuffer::IndexBuffer(int index_size, const char* initial_data, int num_indices, int flags) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();

        m_index_buffer = std::shared_ptr<ALLEGRO_INDEX_BUFFER>(
            al_create_index_buffer(index_size, initial_data, num_indices, flags)  ,
            al_destroy_index_buffer
        );
    }

    void* IndexBuffer::lock(int offset, int length, int flags) { 
        return al_lock_index_buffer(m_index_buffer.get(), offset, length, flags);
    }

    void IndexBuffer::unlock() { 
        al_unlock_index_buffer(m_index_buffer.get());
    }

    int IndexBuffer::size() const { 
        return al_get_index_buffer_size(m_index_buffer.get());
    }

    IndexBuffer::operator ALLEGRO_INDEX_BUFFER*() const { 
        return m_index_buffer.get();
    }

    // VertexArrayType:

    VertexArrayType::VertexArrayType(int type, std::optional<Bitmap> bitmap)
        : m_bitmap(bitmap), m_type(type)
    {}
    
    int VertexArrayType::get_type() const {
        return m_type;
    }

    std::optional<Bitmap> VertexArrayType::get_bitmap() const {
        return m_bitmap;
    }

    void VertexArrayType::set_type(int type) {
        m_type = type;
    }

    void VertexArrayType::set_bitmap(Bitmap bitmap) {
        m_bitmap = bitmap;
    }

    void VertexArrayType::unset_bitmap() {
        m_bitmap = std::nullopt;
    }

    // VertexVector:

    VertexVector::VertexVector(std::vector<Vertex> vertexes, int type, std::optional<Bitmap> bitmap)
        : VertexArrayType(type, bitmap), m_vertexes(vertexes)
    {}

    void VertexVector::recreate(std::vector<Vertex> vertexes, int type, std::optional<Bitmap> bitmap) {
        m_vertexes = vertexes;
        set_type(type);
        if (bitmap.has_value()) set_bitmap(bitmap.value());
        else unset_bitmap();
    }
    
    int VertexVector::draw(int start, int end, const ALLEGRO_VERTEX_DECL* decl) const {
        return al_draw_prim(
            m_vertexes.data(),
            decl,
            m_bitmap.has_value() ? m_bitmap.value() : nullptr,
            start < 0 ? 0 : start,
            end < 0 ? static_cast<int>(m_vertexes.size()) : end,
            m_type
        );
    }

    int VertexVector::draw(const int* indices, int num_vtx, const ALLEGRO_VERTEX_DECL* decl) const {
        return al_draw_indexed_prim(
            m_vertexes.data(),
            decl,
            m_bitmap.has_value() ? m_bitmap.value() : nullptr,
            indices,
            num_vtx,
            m_type
        );
    }

    // VertexBuffer:

    VertexBuffer::VertexBuffer(std::span<Vertex> vertexes, int type, int flags, std::optional<Bitmap> bitmap, ALLEGRO_VERTEX_DECL* decl)
        : VertexArrayType(type, bitmap), m_buffer(al_create_vertex_buffer(
            decl,
            vertexes.data(),
            vertexes.size(),
            flags
        ), al_destroy_vertex_buffer)
    {}
    
    void VertexBuffer::recreate(std::span<Vertex> vertexes, int type, int flags, std::optional<Bitmap> bitmap, ALLEGRO_VERTEX_DECL* decl) {
        m_buffer = std::shared_ptr<ALLEGRO_VERTEX_BUFFER>(al_create_vertex_buffer(
            decl,
            vertexes.data(),
            vertexes.size(),
            flags
        ), al_destroy_vertex_buffer);
        set_type(type);
        if (bitmap.has_value()) set_bitmap(bitmap.value());
        else unset_bitmap();
    }

    void* VertexBuffer::lock(int offset, int length, int flags) {
        return al_lock_vertex_buffer(m_buffer.get(), offset, length, flags);
    }

    void VertexBuffer::unlock() {
        return al_unlock_vertex_buffer(m_buffer.get());
    }

    int VertexBuffer::size() const {
        return al_get_vertex_buffer_size(m_buffer.get());
    }

    int VertexBuffer::draw(int start, int end, const ALLEGRO_VERTEX_DECL* decl) const {
        return al_draw_vertex_buffer(
            m_buffer.get(),
            m_bitmap.has_value() ? m_bitmap.value() : nullptr,
            start < 0 ? 0 : start,
            end < 0 ? size() : end,
            m_type
        );
    }

    int VertexBuffer::draw(ALLEGRO_INDEX_BUFFER* index_buffer, int start, int end) const {
        return al_draw_indexed_buffer(
            m_buffer.get(),
            m_bitmap.has_value() ? m_bitmap.value() : nullptr,
            index_buffer,
            start < 0 ? 0 : start,
            end < 0 ? size() : end,
            m_type
        );
    }










    /*VertexArray VertexArray::create(std::span<Vertex> vertexes, type type) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();

        return VertexArray(
            vertex_copy_vector(vertexes.begin(), vertexes.end()),
            type
        );
    }

    VertexArray VertexArray::create_on_gpu(std::span<Vertex> vertexes, type type, int flags) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
     
        return VertexArray(al_create_vertex_buffer(
            nullptr,
            vertexes.data(),
            vertexes.size(),
            flags
        ), vertexes.size(), type);
    }




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
    }*/
} // namespace Graphics
} // namespace LSWE