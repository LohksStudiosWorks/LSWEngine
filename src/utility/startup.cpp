#include <LSWE/utility/startup.hpp>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>

#include <cstdlib>
#include <atomic>

#include <LSWE/exception/utility_exception.hpp>

namespace LSWE {
namespace Utility {

    static void set_env_cp(const char* name, const char* value) {
#if defined(_WIN32)
            _putenv_s(name, value);
#else
            setenv(name, value, 1);
#endif
    }

    void AllegroInit::setup() {
        static std::atomic_bool ran = false;
        if (ran) return;
        
#ifndef _WIN32
        set_env_cp("GDK_BACKEND", "x11");
#endif

        if (!al_init())
            throw UtilityException("Could not start Allegro system");

        ran = true;
    }

    void AllegroInitPrimitives::setup() {
        static std::atomic_bool ran = false;
        if (ran) return;

        if (!al_init_primitives_addon())
            throw UtilityException("Could not start Allegro primitives");
            
        ran = true;
    }

    void AllegroInitImage::setup() {
        static std::atomic_bool ran = false;
        if (ran) return;
        
        if (!al_init_image_addon())
            throw UtilityException("Could not start Allegro image addon");
            
        ran = true;
    }

    void AllegroInitFont::setup() {
        static std::atomic_bool ran = false;
        if (ran) return;
        
        if (!al_init_font_addon())
            throw UtilityException("Could not start Allegro font addon");
            
        ran = true;
    }

    void AllegroInitTTF::setup() {
        static std::atomic_bool ran = false;
        if (ran) return;
        
        if (!al_init_ttf_addon())
            throw UtilityException("Could not start Allegro TTF addon");
            
        ran = true;
    }

    void AllegroInitDialog::setup() {
        static std::atomic_bool ran = false;
        if (ran) return;

        if (!al_init_native_dialog_addon())
            throw UtilityException("Could not start Allegro native dialog addon");
            
        ran = true;
    }

    void AllegroInitAudio::setup() {
        static std::atomic_bool ran = false;
        if (ran) return;
        
        if (!al_install_audio())
            throw UtilityException("Could not start Allegro audio addon");
            
        ran = true;
    }

    void AllegroInitKeyboard::setup() {
        static std::atomic_bool ran = false;
        if (ran) return;
        
        if (!al_install_keyboard())
            throw UtilityException("Could not start Allegro keyboard");
            
        ran = true;
    }

    void AllegroInitMouse::setup() {
        static std::atomic_bool ran = false;
        if (ran) return;
        
        if (!al_install_mouse())
            throw UtilityException("Could not start Allegro mouse");
            
        ran = true;
    }

    void AllegroInitJoystick::setup() {
        static std::atomic_bool ran = false;
        if (ran) return;
        
        if (!al_install_joystick()) 
            throw UtilityException("Could not start Allegro joystick");
            
        ran = true;
    }

    void AllegroInitTouch::setup() {
        static std::atomic_bool ran = false;
        if (ran) return;
        
        if (!al_install_touch_input())
            throw UtilityException("Could not start Allegro touch");
            
        ran = true;
    }

} // namespace LSWE
} // namespace Utility