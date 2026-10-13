#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>
#include <battery/embed.hpp>

using namespace LSWE;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    const auto jpg_embedded = b::embed<"resources/vrchat_ex.jpg">();

    auto fp = Utility::File::open_mem((void*)jpg_embedded.data(), jpg_embedded.length(), "rb");

    Graphics::MonitorInfo moninfo;

    Graphics::Display::set_new_window_title("Fancy test");
    auto display = Graphics::Display::create(moninfo.get_width() * 0.75f, moninfo.get_height() * 0.75f);
    auto bitmap = Graphics::Bitmap::load(fp, ".jpg");

    const float scales[2] = {
        1.0f * display.get_width() / bitmap.get_width(),
        1.0f * display.get_height() / bitmap.get_height(),
    };
    const float min = std::min(scales[0], scales[1]);

    display->clear(al_map_rgb(50, 50, 50));

    bitmap.draw(bitmap.get_width() / 2, bitmap.get_height() / 2, display.get_width() / 2, display.get_height() / 2,
        0.8f * min, 0.8f * min, 0.0f, 0);
    
    display.flip();

    al_rest(1.0);    

    std::cout << "PASSED!" << std::endl;
    return 0;
}