#pragma once

#include <allegro5/allegro5.h>

#include <memory>
#include <optional>

namespace LSWE {
namespace Events {

    class EventQueue {
    public:
        static EventQueue create();

        void destroy();

        EventQueue& register_event(ALLEGRO_EVENT_SOURCE* source);
        EventQueue& unregister_event(ALLEGRO_EVENT_SOURCE* source);
        bool get_is_registered_event(ALLEGRO_EVENT_SOURCE* source) const;

        void pause(bool pause);
        bool get_is_paused() const;

        bool get_is_empty() const;
        std::optional<ALLEGRO_EVENT> get_next();
        std::optional<ALLEGRO_EVENT> peek_next();
        bool drop_next();
        void flush();
        ALLEGRO_EVENT wait();
        std::optional<ALLEGRO_EVENT> wait(float seconds);
        std::optional<ALLEGRO_EVENT> wait(ALLEGRO_TIMEOUT&& timeout);

        EventQueue& operator<<(ALLEGRO_EVENT_SOURCE* source);

        operator ALLEGRO_EVENT_QUEUE*() const;
    private:
        EventQueue(ALLEGRO_EVENT_QUEUE* ev_q);

        std::shared_ptr<ALLEGRO_EVENT_QUEUE> m_event_queue;
    };

} // namespace Events
} // namespace LSWE