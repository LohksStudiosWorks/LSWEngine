#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <memory>
#include <optional>
#include <span>

#include <LSWE/graphics/bitmap.hpp>
#include <LSWE/graphics/transform.hpp>

namespace LSWE {
namespace Graphics {

    struct Vertex : public ALLEGRO_VERTEX {
        Vertex() = default;

		Vertex(float x, float y, ALLEGRO_COLOR c);
		Vertex(float x, float y, float z, ALLEGRO_COLOR c);
		Vertex(float x, float y, float z, float u, float v, ALLEGRO_COLOR c);
    };

    class Vertexes {
    public:
		enum class type {
			POINT_LIST		= ALLEGRO_PRIM_POINT_LIST,			// Each vertex is a point, literally.
			LINE_LIST		= ALLEGRO_PRIM_LINE_LIST,			// Pairs of points defining lines. Expects pair of vertex points. Each pair is a line.
			LINE_STRIP		= ALLEGRO_PRIM_LINE_STRIP,			// Combo of points drawing a complex line. Every two points create a line.
			LINE_LOOP		= ALLEGRO_PRIM_LINE_LOOP,			// Combo of points like LINE_STRIP but the last point also connects to the first one
			TRIANGLE_LIST	= ALLEGRO_PRIM_TRIANGLE_LIST,		// Triangle list (each three vertex points make a triangle). Expects multiple of 3.
			TRIANGLE_STRIP	= ALLEGRO_PRIM_TRIANGLE_STRIP,		// Triangle strip is a "keep going" triangle list. 3 points make one triangle one by one ([0,1,2], [1,2,3], [2,3,4], ...)
			TRIANGLE_FAN	= ALLEGRO_PRIM_TRIANGLE_FAN			// A "center" point and triangles around. All triangles share the first vertex point.
		};
        
        static Vertexes create(std::span<Vertex> vertexes, type type, int flags = ALLEGRO_PRIM_BUFFER_STATIC, std::optional<Bitmap> texture = std::nullopt);

        void replace(std::span<Vertex> vertexes, type type, int flags = ALLEGRO_PRIM_BUFFER_STATIC);

        void set_texture(Bitmap bitmap);
        void remove_texture();

        size_t size() const;
        void draw() const;
        void draw(size_t from, size_t to);
        void draw(size_t from, size_t to, Bitmap with_texture);

        Transform& get_transform();
        Transform* operator->();
    private:
        Vertexes(ALLEGRO_VERTEX_BUFFER* gpu_vertexes, size_t len, type type, std::optional<Bitmap> bitmap);

        static ALLEGRO_VERTEX_BUFFER* generate_gpu_buffer(const ALLEGRO_VERTEX* data, size_t len, int flags);

        std::shared_ptr<ALLEGRO_VERTEX_BUFFER> m_gpu_vertexes;
        std::optional<Bitmap> m_atlas_texture;
        Transform m_object_positioning;

        size_t m_vertex_count;
        type m_type;
    };
    
} // namespace Graphics
} // namespace LSWE