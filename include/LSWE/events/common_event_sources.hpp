#pragma once

#include <allegro5/allegro5.h>

#include <memory>
#include <optional>

namespace LSWE {
namespace Events {

    class GenericEventSource {
    public:
        virtual operator ALLEGRO_EVENT_SOURCE*() const = 0;
    };

    class KeyboardEventSource : public GenericEventSource {
    public:
        KeyboardEventSource();
        operator ALLEGRO_EVENT_SOURCE*() const override;
    };

    class MouseEventSource : public GenericEventSource {
    public:
        MouseEventSource();
        operator ALLEGRO_EVENT_SOURCE*() const override;
    };

    class TouchEventSource : public GenericEventSource {
    public:
        TouchEventSource();
        operator ALLEGRO_EVENT_SOURCE*() const override;
    };

    class JoystickEventSource : public GenericEventSource {
    public:
        JoystickEventSource();
        operator ALLEGRO_EVENT_SOURCE*() const override;
    };

} // namespace Events
} // namespace LSWE