#pragma once

#include <allegro5/allegro5.h>

#include <memory>
#include <optional>

namespace LSWE {
namespace Events {

    class UserEvent {
    public:
        static UserEvent create();

        void set_data(intptr_t data);
        intptr_t get_data();

        bool emit(ALLEGRO_EVENT ev, void (*data_destructor)(ALLEGRO_USER_EVENT*));

        // must be called on receive on the other side
        static void unref(ALLEGRO_EVENT& received_ev);

        operator ALLEGRO_EVENT_SOURCE*() const;
    private:
        UserEvent(ALLEGRO_EVENT_SOURCE* event_source);

        std::shared_ptr<ALLEGRO_EVENT_SOURCE> m_event_source;
    };

} // namespace Events
} // namespace LSWE