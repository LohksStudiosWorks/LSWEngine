#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>
#include <battery/embed.hpp>

using namespace LSWE;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    const auto jpg_embedded = b::embed<"resources/my_gif.gif">();

    auto fp = Utility::File::open_mem((void*)jpg_embedded.data(), jpg_embedded.length(), "rb");

    Graphics::Display::set_new_window_title("Fancy test");
    auto display = Graphics::Display::create(1280, 720);
    auto font = Graphics::Font::create_builtin();

    display->clear(al_map_rgb(50, 50, 50));
    constexpr char loading_str[] = "Loading GIF, please wait...";
    const auto s_width = font.get_width(loading_str);
    const auto s_height = font.get_line_height();

    font.draw(al_map_rgb(235,235,235), (display.get_width() - s_width) / 2, (display.get_height() - s_height) / 2, 0, loading_str);
    display.flip();

    auto bitmap = Graphics::GIF::load(fp);

    const float scales[2] = {
        1.0f * display.get_width() / bitmap.get_width(),
        1.0f * display.get_height() / bitmap.get_height(),
    };
    const float min = std::min(scales[0], scales[1]);

    for(double noww = al_get_time() + 10.0; noww - al_get_time() > 0.0;) {
        display->clear(al_map_rgb(50, 50, 50));

        bitmap.draw(bitmap.get_width() / 2, bitmap.get_height() / 2, display.get_width() / 2, display.get_height() / 2,
            0.8f * min, 0.8f * min, 0.0f, 0);
        
        display.flip();
    }

    al_rest(1.0);
    

    std::cout << "PASSED!" << std::endl;
    return 0;
}