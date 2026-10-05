#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>
#include <any>

using namespace LSWE::Utility;

int main(int argc, char **argv) {
    try {
        std::any tests[] = {
            SingletonOf<AllegroInit>::instance(),
            SingletonOf<AllegroInitPrimitives>::instance(),
            SingletonOf<AllegroInitImage>::instance(),
            SingletonOf<AllegroInitFont>::instance(),
            SingletonOf<AllegroInitTTF>::instance(),
            SingletonOf<AllegroInitDialog>::instance(),
            SingletonOf<AllegroInitAudio>::instance(),
            SingletonOf<AllegroInitKeyboard>::instance(),
            SingletonOf<AllegroInitMouse>::instance(),
            SingletonOf<AllegroInitJoystick>::instance()
            // SingletonOf<AllegroInitTouch>::instance() // I don't have touch features, so this fails :x
        };
    }
    catch(const std::exception& any_except) {
        std::cerr << "Failed to instantiate at least one Allegro feature." << std::endl;
        std::cerr << "Details: " << any_except.what() << std::endl;
        return 1;
    }

    std::cout << "PASSED!" << std::endl;
    return 0;
}
