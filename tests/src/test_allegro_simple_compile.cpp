#include <allegro5/allegro5.h>
#include <iostream>

int main() {
    al_init();

    ALLEGRO_DISPLAY *display = al_create_display(800, 600);
    if (!display) {
        std::cerr << "Failed to create display.\n";
        return -1;
    }

    al_clear_to_color(al_map_rgb(50, 100, 200));
    al_flip_display();
    al_rest(0.2);

    al_destroy_display(display);

    std::cout << "PASSED!" << std::endl;
    return 0;
}
