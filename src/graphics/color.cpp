#include <LSWE/graphics/color.hpp>

namespace LSWE {
namespace Graphics {

    Color::Color(float r, float g, float b, float a)
        : ALLEGRO_COLOR({
            .r = r,
            .g = g,
            .b = b,
            .a = a
        })
    {}

    Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
        : ALLEGRO_COLOR({
            .r = static_cast<float>(r) / 255.0f,
            .g = static_cast<float>(g) / 255.0f,
            .b = static_cast<float>(b) / 255.0f,
            .a = static_cast<float>(a) / 255.0f
        })
    {}

    float Color::get_r() const { 
        return this->r;
    }

    float Color::get_g() const { 
        return this->g;
    }

    float Color::get_b() const { 
        return this->b;
    }

    float Color::get_a() const { 
        return this->a;
    }

    void Color::clear_to_this() const { 
        al_clear_to_color(*this);
    }

} // namespace Graphics
} // namespace LSWE