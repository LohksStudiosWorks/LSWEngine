#include <LSWE/events/timer.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Events {

    Timer Timer::create(double seconds) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();

        return Timer(al_create_timer(seconds));
    }

    void Timer::destroy() {
        m_timer.reset();
    }

    void Timer::restart() {
        stop();
        start();
    }

    void Timer::start() {
        al_start_timer(m_timer.get());
    }

    void Timer::resume() {
        al_resume_timer(m_timer.get());
    }

    void Timer::stop() {
        al_stop_timer(m_timer.get());
    }

    bool Timer::get_started() const {
        return al_get_timer_started(m_timer.get());
    }

    int64_t Timer::get_count() const {
        return al_get_timer_count(m_timer.get());
    }

    double Timer::get_speed() {
        return al_get_timer_speed(m_timer.get());
    }

    void Timer::set_count(int64_t times) {
        al_set_timer_count(m_timer.get(), times);
    }

    void Timer::add_count(int64_t times) {
        al_add_timer_count(m_timer.get(), times);
    }

    void Timer::set_speed(double seconds) {
        al_set_timer_speed(m_timer.get(), seconds);
    }

    ALLEGRO_EVENT_SOURCE* Timer::get_event_source() const {
        return al_get_timer_event_source(m_timer.get());
    }
    
    Timer::operator ALLEGRO_EVENT_SOURCE*() const {
        return al_get_timer_event_source(m_timer.get());        
    }
    
    Timer::operator ALLEGRO_TIMER*() const {
        return m_timer.get();        
    }

    Timer::Timer(ALLEGRO_TIMER* timer)
        : m_timer(timer, al_destroy_timer)
    {
        if (!m_timer)
            throw Exception::NullException("Timer was null");
    }
    
} // namespace Events
} // namespace LSWE