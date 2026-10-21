#pragma once

#include <allegro5/allegro5.h>

#include <memory>

namespace LSWE {
namespace Events {

    class Timer {
    public:
        static Timer create(double seconds);

        void destroy();

        void restart();
        void start();
        void resume();
        void stop();

        bool get_started() const;
        int64_t get_count() const;
        double get_speed();

        void set_count(int64_t times);
        void add_count(int64_t times);
        void set_speed(double seconds);

        ALLEGRO_EVENT_SOURCE* get_event_source() const;
        
        operator ALLEGRO_EVENT_SOURCE*() const;
        operator ALLEGRO_TIMER*() const;
    private:
        Timer(ALLEGRO_TIMER* timer);

        std::shared_ptr<ALLEGRO_TIMER> m_timer;
    };

} // namespace Events
} // namespace LSWE