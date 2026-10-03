#include "LSWE/test.h"

ALLEGRO_DISPLAY* bdah() {
    al_init();
    return al_create_display(800, 600);
}

void dodah() {
    al_clear_to_color(al_map_rgb(50, 100, 200));
    al_flip_display();
    al_rest(2.0);
}

void ddah(ALLEGRO_DISPLAY* d) {
    al_destroy_display(d);
}
