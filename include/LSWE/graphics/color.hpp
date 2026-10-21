#pragma once

#include <allegro5/allegro5.h>

namespace LSWE {
namespace Graphics {

    class Color : public ALLEGRO_COLOR {
    public:
		Color() = default;
		Color(float r, float g, float b, float a = 1.0);
		Color(int r, int g, int b, int a = 255);
		Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

		float get_r() const;
		float get_g() const;
		float get_b() const;
		float get_a() const;

		void clear_to_this() const;
    };
    
} // namespace Graphics
} // namespace LSWE