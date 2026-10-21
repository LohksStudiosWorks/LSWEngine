#include <LSWE/lswe.hpp>
#include <allegro5/allegro5.h>
#include <iostream>

using namespace LSWE;

int main() {
    al_init();

    auto display = Graphics::Display::create(800, 600);
    auto bitmap = Graphics::Bitmap::create(800, 600);

    if (Graphics::Bitmap::get_target() != display.get_backbuffer()) {
        std::cerr << "Backbuffer was not target.\n";
        return -1;
    }

    {
        Utility::MakeTemporaryTarget temp_target(bitmap);

        if (Graphics::Bitmap::get_target() == display.get_backbuffer()) {
            std::cerr << "Backbuffer wasn't supposed to be target.\n";
            return -1;
        }
        if (Graphics::Bitmap::get_target() != bitmap) {
            std::cerr << "Target was not set to bitmap.\n";
            return -1;
        }

        Graphics::Color(100, 50, 25).clear_to_this();
    }

    if (Graphics::Bitmap::get_target() != display.get_backbuffer()) {
        std::cerr << "Backbuffer not as target after block.\n";
        return -1;
    }

    bitmap.draw(0, 0, 0);

    al_flip_display();
    al_rest(0.5);

    std::cout << "PASSED!" << std::endl;
    return 0;
}
