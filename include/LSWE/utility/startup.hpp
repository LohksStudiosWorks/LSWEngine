#pragma once

#include <LSWE/utility/singleton.hpp>

namespace LSWE {
namespace Utility {

    /**
     * @brief Base init of Allegro 5
     */
    MAKE_SINGLETON_CLASS_NAMED(AllegroInit);


    /// GRAPHICS

    /**
     * @brief Init primitives addon
     */
    MAKE_SINGLETON_CLASS_NAMED(AllegroInitPrimitives);

    /**
     * @brief Init image addon
     */
    MAKE_SINGLETON_CLASS_NAMED(AllegroInitImage);

    /**
     * @brief Init font addon
     */
    MAKE_SINGLETON_CLASS_NAMED(AllegroInitFont);

    /**
     * @brief Init TTF addon
     */
    MAKE_SINGLETON_CLASS_NAMED(AllegroInitTTF);

    /**
     * @brief Init dialog addon
     */
    MAKE_SINGLETON_CLASS_NAMED(AllegroInitDialog);


    /// AUDIO

    /**
     * @brief Base for all audio setup. Single setup though
     */
    MAKE_SINGLETON_CLASS_NAMED(AllegroInitAudio);


    /// EVENTS

    /**
     * @brief Init keyboard addon
     */
    MAKE_SINGLETON_CLASS_NAMED(AllegroInitKeyboard);

    /**
     * @brief Init mouse addon
     */
    MAKE_SINGLETON_CLASS_NAMED(AllegroInitMouse);

    /**
     * @brief Init joystick addon
     */
    MAKE_SINGLETON_CLASS_NAMED(AllegroInitJoystick);

    /**
     * @brief Init touch addon
     */
    MAKE_SINGLETON_CLASS_NAMED(AllegroInitTouch);
    
} // namespace LSWE
} // namespace Utility