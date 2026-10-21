#include <LSWE/events/event_queue.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Events {

    EventQueue EventQueue::create() {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();

        return EventQueue(al_create_event_queue());
    }

    void EventQueue::destroy() {
        m_event_queue.reset();
    }

    EventQueue& EventQueue::register_event(ALLEGRO_EVENT_SOURCE* source) {
        al_register_event_source(m_event_queue.get(), source);
        return *this;
    }

    EventQueue& EventQueue::unregister_event(ALLEGRO_EVENT_SOURCE* source) {
        al_unregister_event_source(m_event_queue.get(), source);
        return *this;
    }

    bool EventQueue::get_is_registered_event(ALLEGRO_EVENT_SOURCE* source) const {
        return al_is_event_source_registered(m_event_queue.get(), source);
    }

    void EventQueue::pause(bool pause) {
        al_pause_event_queue(m_event_queue.get(), pause);
    }

    bool EventQueue::get_is_paused() const {
        return al_is_event_queue_paused(m_event_queue.get());
    }

    bool EventQueue::get_is_empty() const {
        return al_is_event_queue_empty(m_event_queue.get());
    }

    std::optional<ALLEGRO_EVENT> EventQueue::get_next() {
        ALLEGRO_EVENT ev;
        if (al_get_next_event(m_event_queue.get(), &ev))
            return ev;
        return std::nullopt;
    }

    std::optional<ALLEGRO_EVENT> EventQueue::peek_next() {
        ALLEGRO_EVENT ev;
        if (al_peek_next_event(m_event_queue.get(), &ev))
            return ev;
        return std::nullopt;
    }

    bool EventQueue::drop_next() {
        return al_drop_next_event(m_event_queue.get());
    }

    void EventQueue::flush() {
        al_flush_event_queue(m_event_queue.get());
    }

    ALLEGRO_EVENT EventQueue::wait() {
        ALLEGRO_EVENT ev;
        al_wait_for_event(m_event_queue.get(), &ev);
        return ev;
    }

    std::optional<ALLEGRO_EVENT> EventQueue::wait(float seconds) {
        ALLEGRO_EVENT ev;
        if (al_wait_for_event_timed(m_event_queue.get(), &ev, seconds))
            return ev;
        return std::nullopt;
    }

    std::optional<ALLEGRO_EVENT> EventQueue::wait(ALLEGRO_TIMEOUT&& timeout) {
        ALLEGRO_EVENT ev;
        if (al_wait_for_event_until(m_event_queue.get(), &ev, &timeout))
            return ev;
        return std::nullopt;            
    }
    
    EventQueue& EventQueue::operator<<(ALLEGRO_EVENT_SOURCE* source) {
        al_register_event_source(m_event_queue.get(), source);
        return *this;
    }

    EventQueue::operator ALLEGRO_EVENT_QUEUE*() const {
        return m_event_queue.get();
    }
    
    EventQueue::EventQueue(ALLEGRO_EVENT_QUEUE* ev_q) 
        : m_event_queue(ev_q, al_destroy_event_queue)
    {
        if (!m_event_queue)
            throw Exception::NullException("Event queue was null");
    }
    
} // namespace Events
} // namespace LSWE