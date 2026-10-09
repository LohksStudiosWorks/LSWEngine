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

    auto display = Graphics::Display::create(1280, 720);
    auto bitmap = Graphics::Bitmap::load(fp, ".jpg");

    display->clear(al_map_rgb(50, 50, 50));

    bitmap.draw(bitmap.get_width() / 2, bitmap.get_height() / 2, display.get_width() / 2, display.get_height() / 2,
        display.get_width() * 0.8f / bitmap.get_width(), display.get_height() * 0.8f / bitmap.get_height(), 0.0f, 0);
    
    display.flip();

    al_rest(10.0);    

    return 0;
}