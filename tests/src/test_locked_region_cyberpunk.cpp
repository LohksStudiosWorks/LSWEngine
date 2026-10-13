#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>
#include <cmath>
#include <battery/embed.hpp>

using namespace LSWE;

void manage_image(Graphics::LockedRegion origin, Graphics::LockedRegion target, const Graphics::Bitmap& bmp);

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    const auto jpg_embedded = b::embed<"resources/vrchat_ex.jpg">();

    auto fp = Utility::File::open_mem((void*)jpg_embedded.data(), jpg_embedded.length(), "rb");

    Graphics::MonitorInfo moninfo;

    Graphics::Bitmap::set_new_format(ALLEGRO_PIXEL_FORMAT_RGBA_8888);
    Graphics::Bitmap::set_new_flags(ALLEGRO_MAG_LINEAR | ALLEGRO_MIN_LINEAR);

    Graphics::Display::set_new_window_title("Fancy test");
    auto display = Graphics::Display::create(moninfo.get_width() * 0.75f, moninfo.get_height() * 0.75f);
    auto bitmap_orig = Graphics::Bitmap::load(fp, ".jpg");

    auto bitmap = Graphics::Bitmap::create(display.get_width() * 0.3f, display.get_height() * 0.3f);
    {
        Utility::MakeTemporaryTarget target(bitmap);
        bitmap_orig.draw(0, 0, bitmap_orig.get_width(), bitmap_orig.get_height(),
            0, 0, bitmap.get_width(), bitmap.get_height(), 0);
    }

    auto cpy = Graphics::Bitmap::create(bitmap.get_width(), bitmap.get_height());

    display->clear(al_map_rgb(50, 50, 50));

    const float scales[2] = {
        1.0f * display.get_width() / cpy.get_width(),
        1.0f * display.get_height() / cpy.get_height(),
    };
    const float min = std::min(scales[0], scales[1]);

    manage_image(
        Graphics::LockedRegion(bitmap, ALLEGRO_PIXEL_FORMAT_RGBA_8888, ALLEGRO_LOCK_READONLY),
        Graphics::LockedRegion(cpy, ALLEGRO_PIXEL_FORMAT_RGBA_8888, ALLEGRO_LOCK_WRITEONLY),
        bitmap);

    cpy.draw(cpy.get_width() / 2, cpy.get_height() / 2, display.get_width() / 2, display.get_height() / 2,
        0.8f * min, 0.8f * min, 0.0f, 0);
    
    display.flip();

    al_rest(5.0);

    std::cout << "PASSED!" << std::endl;
    return 0;
}

void manage_image(Graphics::LockedRegion origin, Graphics::LockedRegion target, const Graphics::Bitmap& bmp) {
    const int width = bmp.get_width();
    const int height = bmp.get_height();

    const auto get_pixel_at = [&origin, width, height](int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) return al_map_rgb(0,0,0);

        const unsigned char *row = (unsigned char*)origin.get_data() + (y * origin.get_pitch());
        const int idx = x * origin.get_pixel_size();

        return al_map_rgb(row[idx + 0], row[idx + 1], row[idx + 2]);
    };
    const auto set_pixel_at = [&target, width, height](int x, int y, ALLEGRO_COLOR color) {
        if (x < 0 || x >= width || y < 0 || y >= height) return;

        unsigned char *row = (unsigned char*)target.get_data() + (y * target.get_pitch());
        const int idx = x * target.get_pixel_size();
        
        row[idx + 0] = static_cast<uint8_t>(color.r * 255.0f);
        row[idx + 1] = static_cast<uint8_t>(color.g * 255.0f);
        row[idx + 2] = static_cast<uint8_t>(color.b * 255.0f);
    };

    for (int y = 0; y < height; y++) {                
        for (int x = 0; x < width; x++) {
            const ALLEGRO_COLOR sides[5] = {
                get_pixel_at(x - 1, y),
                get_pixel_at(x, y - 1),
                get_pixel_at(x + 1, y),
                get_pixel_at(x, y + 1),
                get_pixel_at(x, y)
            };

            set_pixel_at(x, y, al_map_rgb_f(
                0.8f * sides[0].r + 0.2f * sides[2].r,
                0.4f * sides[1].g + 0.6f * sides[3].g,
                0.3f * sides[0].b + 0.5f * sides[1].b + 0.2f * sides[2].b
            ));
            
        }
    }
}