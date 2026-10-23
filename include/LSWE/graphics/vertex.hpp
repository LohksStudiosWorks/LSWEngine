#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <memory>
#include <optional>
#include <vector>
#include <span>

#include <LSWE/graphics/bitmap.hpp>
#include <LSWE/graphics/transform.hpp>

namespace LSWE {
namespace Graphics {

/*
    ALLEGRO_PRIM_POINT_LIST,			// Each vertex is a point, literally.
    ALLEGRO_PRIM_LINE_LIST,			    // Pairs of points defining lines. Expects pair of vertex points. Each pair is a line.
    ALLEGRO_PRIM_LINE_STRIP,			// Combo of points drawing a complex line. Every two points create a line.
    ALLEGRO_PRIM_LINE_LOOP,			    // Combo of points like LINE_STRIP but the last point also connects to the first one
    ALLEGRO_PRIM_TRIANGLE_LIST,		    // Triangle list (each three vertex points make a triangle). Expects multiple of 3.
    ALLEGRO_PRIM_TRIANGLE_STRIP,		// Triangle strip is a "keep going" triangle list. 3 points make one triangle one by one ([0,1,2], [1,2,3], [2,3,4], ...)
    ALLEGRO_PRIM_TRIANGLE_FAN			// A "center" point and triangles around. All triangles share the first vertex point.
*/

    struct Vertex : public ALLEGRO_VERTEX {
        Vertex();

		Vertex(float x, float y, ALLEGRO_COLOR c);
		Vertex(float x, float y, float z, ALLEGRO_COLOR c);
		Vertex(float x, float y, float z, float u, float v, ALLEGRO_COLOR c);
    };

    class VertexDecl {
    public:
        VertexDecl(const ALLEGRO_VERTEX_ELEMENT* elements, int size_stride);

        operator ALLEGRO_VERTEX_DECL*() const;
    private:
        std::shared_ptr<ALLEGRO_VERTEX_DECL> m_decl;
    };

    class IndexBuffer {
    public:
        IndexBuffer(int index_size, const char* initial_data, int num_indices, int flags);

        void* lock(int offset, int length, int flags);
        void unlock();

        int size() const;

        operator ALLEGRO_INDEX_BUFFER*() const;
    private:
        std::shared_ptr<ALLEGRO_INDEX_BUFFER> m_index_buffer;
    };

    class VertexArrayType {
    public:
        VertexArrayType(int type = ALLEGRO_PRIM_TRIANGLE_STRIP, std::optional<Bitmap> bitmap = std::nullopt);

        int get_type() const;
        std::optional<Bitmap> get_bitmap() const;

        void set_type(int type);
        void set_bitmap(Bitmap bitmap);
        void unset_bitmap();

        virtual int draw(int start = -1, int end = -1, const ALLEGRO_VERTEX_DECL* decl = nullptr) const = 0;
    protected:
        std::optional<Bitmap> m_bitmap;
        int m_type;
    };

    class VertexVector : public VertexArrayType {
    public:
        VertexVector(std::vector<Vertex> vertexes, int type = ALLEGRO_PRIM_TRIANGLE_STRIP, std::optional<Bitmap> bitmap = std::nullopt);

        void recreate(std::vector<Vertex> vertexes, int type = ALLEGRO_PRIM_TRIANGLE_STRIP, std::optional<Bitmap> bitmap = std::nullopt);
        
        int draw(int start = -1, int end = -1, const ALLEGRO_VERTEX_DECL* decl = nullptr) const override;
        int draw(const int* indices, int num_vtx, const ALLEGRO_VERTEX_DECL* decl = nullptr) const;
    private:
        std::vector<Vertex> m_vertexes;
    };

    class VertexBuffer : public VertexArrayType {
    public:
        VertexBuffer(std::span<Vertex> vertexes, int type = ALLEGRO_PRIM_TRIANGLE_STRIP, int flags = 0, std::optional<Bitmap> bitmap = std::nullopt, ALLEGRO_VERTEX_DECL* decl = nullptr);

        void recreate(std::span<Vertex> vertexes, int type = ALLEGRO_PRIM_TRIANGLE_STRIP, int flags = 0, std::optional<Bitmap> bitmap = std::nullopt, ALLEGRO_VERTEX_DECL* decl = nullptr);

        void* lock(int offset, int length, int flags);
        void unlock();

        int size() const;

        int draw(int start = -1, int end = -1, const ALLEGRO_VERTEX_DECL* decl = nullptr) const override;
        int draw(ALLEGRO_INDEX_BUFFER* index_buffer, int start = -1, int end = -1) const;
    private:
        std::shared_ptr<ALLEGRO_VERTEX_BUFFER> m_buffer;
    };
    
} // namespace Graphics
} // namespace LSWE