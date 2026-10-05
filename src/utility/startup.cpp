#include <LSWE/utility/startup.hpp>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>

#include <LSWE/exception/utility_exception.hpp>

namespace LSWE {
namespace Utility {

    AllegroInit::AllegroInit() {
        if (!al_init())
            throw UtilityException("Could not start Allegro system");
    }

    AllegroInitPrimitives::AllegroInitPrimitives() {
        if (!al_init_primitives_addon())
            throw UtilityException("Could not start Allegro primitives");
    }

    AllegroInitImage::AllegroInitImage() {
        if (!al_init_image_addon())
            throw UtilityException("Could not start Allegro image addon");
    }

    AllegroInitFont::AllegroInitFont() {
        if (!al_init_font_addon())
            throw UtilityException("Could not start Allegro font addon");
    }

    AllegroInitTTF::AllegroInitTTF() {
        if (!al_init_ttf_addon())
            throw UtilityException("Could not start Allegro TTF addon");
    }

    AllegroInitDialog::AllegroInitDialog() {
        if (!al_init_native_dialog_addon()) {
            setenv("GDK_BACKEND", "x11", 1);
            if (!al_init_native_dialog_addon())
                throw UtilityException("Could not start Allegro native dialog addon");
        }
    }

    AllegroInitAudio::AllegroInitAudio() {
        if (!al_install_audio())
            throw UtilityException("Could not start Allegro audio addon");
        reserve_samples(8); // at least 8
    }

    void AllegroInitAudio::reserve_samples(const uint32_t samples) {
        if (!al_reserve_samples(samples))
            throw UtilityException("Could not start Allegro audio: failed to reserve 8");
    }

    AllegroInitKeyboard::AllegroInitKeyboard() {
        if (!al_install_keyboard())
            throw UtilityException("Could not start Allegro keyboard");
    }

    AllegroInitMouse::AllegroInitMouse() {
        if (!al_install_mouse())
            throw UtilityException("Could not start Allegro mouse");
    }

    AllegroInitJoystick::AllegroInitJoystick() {
        if (!al_install_joystick()) 
            throw UtilityException("Could not start Allegro joystick");
    }

    AllegroInitTouch::AllegroInitTouch() {
        if (!al_install_touch_input())
            throw UtilityException("Could not start Allegro touch");
    }

} // namespace LSWE
} // namespace Utility