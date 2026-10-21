#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>
#include <battery/embed.hpp>

#include <allegro5/allegro_ttf.h>

using namespace LSWE;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    const auto font_embedded = b::embed<"resources/calibri_fix.ttf">();

    auto fp = Utility::File::open_mem((void*)font_embedded.data(), font_embedded.length(), "rb");

    Graphics::Display::set_new_window_title("Fancy test");
    auto display = Graphics::Display::create(1280, 720);

    auto font = Graphics::Font::load_ttf(std::move(fp), 24);

    display->clear(al_map_rgb(50, 50, 50));

    font.draw(al_map_rgb(255,255,255), 0, 0, 0, "Hello world");
    
    display.flip();

    al_rest(1.0);    

    std::cout << "PASSED!" << std::endl;
    return 0;
}