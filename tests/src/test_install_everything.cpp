#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>

using namespace LSWE::Utility;

int main(int argc, char *argv[]) {
    
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    try {
        SingletonOf<AllegroInit>::instance().setup();
        SingletonOf<AllegroInitPrimitives>::instance().setup();
        SingletonOf<AllegroInitImage>::instance().setup();
        SingletonOf<AllegroInitFont>::instance().setup();
        SingletonOf<AllegroInitTTF>::instance().setup();
        SingletonOf<AllegroInitDialog>::instance().setup();

        if (platform != "Windows") {
            SingletonOf<AllegroInitAudio>::instance().setup();
        }

        SingletonOf<AllegroInitKeyboard>::instance().setup();
        SingletonOf<AllegroInitMouse>::instance().setup();
        SingletonOf<AllegroInitJoystick>::instance().setup();
        
        // I don't have touch features, so this fails :x
        // SingletonOf<AllegroInitTouch>::instance()
        
    }
    catch(const std::exception& any_except) {
        std::cerr << "Failed to instantiate at least one Allegro feature." << std::endl;
        std::cerr << "Details: " << any_except.what() << std::endl;
        return 1;
    }

    std::cout << "PASSED!" << std::endl;
    return 0;
}
