#include <LSWE/events/user_event.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Events {

    UserEvent UserEvent::create() {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();

        auto* event_source = new ALLEGRO_EVENT_SOURCE();
        al_init_user_event_source(event_source);
        return UserEvent(event_source);
    }

    void UserEvent::set_data(intptr_t data) {
        al_set_event_source_data(m_event_source.get(), data);
    }

    intptr_t UserEvent::get_data() {
        return al_get_event_source_data(m_event_source.get());
    }    

    bool UserEvent::emit(ALLEGRO_EVENT ev, void (*data_destructor)(ALLEGRO_USER_EVENT*)) {
        return al_emit_user_event(m_event_source.get(), &ev, data_destructor);
    }
    
    void UserEvent::unref(ALLEGRO_EVENT& received_ev) {
        al_unref_user_event(&received_ev.user);
    }

    UserEvent::operator ALLEGRO_EVENT_SOURCE*() const {
        return m_event_source.get();
    }
    
    UserEvent::UserEvent(ALLEGRO_EVENT_SOURCE* event_source)
        : m_event_source(event_source, [](auto* s){ al_destroy_user_event_source(s); delete s; })
    {
        if (!m_event_source)
            throw Exception::NullException("Event source was null");
    }

} // namespace Events
} // namespace LSWE