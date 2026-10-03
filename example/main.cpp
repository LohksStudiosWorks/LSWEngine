#include <LSWE/test.h>
#include <allegro5/allegro5.h>
#include <iostream>

int main(int argc, char **argv) {

    // Create a simple window
    ALLEGRO_DISPLAY *display = bdah();
    if (!display) {
        std::cerr << "Failed to create display.\n";
        return -1;
    }

    al_rest(0.6);

    dodah();
    // Clear to a blue color, wait 2 seconds, and exit
    
    ddah(display);
    return 0;
}
