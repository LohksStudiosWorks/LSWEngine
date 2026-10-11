#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>
#include <battery/embed.hpp>

#include <allegro5/allegro_ttf.h>

using namespace LSWE;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    const auto font_embedded = b::embed<"resources/calibri.ttf">();

    //auto fp = Utility::File::open_mem((void*)font_embedded.data(), font_embedded.length(), "rb");


    Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
    Utility::SingletonOf<Utility::AllegroInitFont>::instance().setup();
    Utility::SingletonOf<Utility::AllegroInitTTF>::instance().setup();

    Graphics::Display::set_new_window_title("Fancy test");
    auto display = Graphics::Display::create(1280, 720);

    ALLEGRO_FILE* fp = al_open_memfile((void*)font_embedded.data(), font_embedded.length(), "rb");
    ALLEGRO_FONT* font = al_load_ttf_font_f(fp, "resources/calibri.ttf", 96, 0);


    //auto font = Graphics::Font::load_ttf(fp, 24);

    display->clear(al_map_rgb(50, 50, 50));

    al_draw_text(font, al_map_rgb(255,255,255), 10, 10, 0, "Hello world");

    //font.draw(al_map_rgb(255,255,255), 0, 0, 0, "Hello world");
    
    display.flip();

    al_rest(1.0);    

    std::cout << "PASSED!" << std::endl;
    return 0;
}