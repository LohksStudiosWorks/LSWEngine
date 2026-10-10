#include <LSWE/lswe.hpp>
#include <allegro5/allegro5.h>
#include <iostream>

using namespace LSWE;

int main() {
    al_init();

    ALLEGRO_DISPLAY *display = al_create_display(800, 600);
    if (!display) {
        std::cerr << "Failed to create display.\n";
        return -1;
    }
    ALLEGRO_BITMAP* bitmap = al_create_bitmap(800, 600);
    if (!bitmap) {
        std::cerr << "Failed to create bitmap.\n";
        return -1;
    }

    if (al_get_target_bitmap() != al_get_backbuffer(display)) {
        std::cerr << "Backbuffer was not target.\n";
        return -1;
    }

    {
        Utility::MakeTemporaryTarget temp_target(bitmap);

        if (al_get_target_bitmap() == al_get_backbuffer(display)) {
            std::cerr << "Backbuffer wasn't supposed to be target.\n";
            return -1;
        }
        if (al_get_target_bitmap() != bitmap) {
            std::cerr << "Target was not set to bitmap.\n";
            return -1;
        }

        al_clear_to_color(al_map_rgb(100, 50, 25));
    }

    if (al_get_target_bitmap() != al_get_backbuffer(display)) {
        std::cerr << "Backbuffer not as target after block.\n";
        return -1;
    }

    al_draw_bitmap(bitmap, 0, 0, 0);

    al_flip_display();
    al_rest(0.5);

    al_destroy_bitmap(bitmap);
    al_destroy_display(display);

    std::cout << "PASSED!" << std::endl;
    return 0;
}
