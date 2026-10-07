#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_audio.h>

#include <LSWE/utility/memory.hpp>

namespace LSWE {
namespace Audio {

    class Recorder {
    public:
        static Recorder create(size_t fragment_count, unsigned int frag_samples, unsigned int freq = 48000, ALLEGRO_AUDIO_DEPTH depth = ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF conf = ALLEGRO_CHANNEL_CONF_1);
        
        static ALLEGRO_AUDIO_RECORDER_EVENT* get_from_event(ALLEGRO_EVENT* event);

        void destroy();

        bool start();
        void stop();

        bool get_is_recording() const;
        ALLEGRO_EVENT_SOURCE* get_event_source() const;

        operator ALLEGRO_EVENT_SOURCE*() const;
        operator ALLEGRO_AUDIO_RECORDER*();
    private:
        Recorder(ALLEGRO_AUDIO_RECORDER*&& recorder);

        Utility::LazyPointer<ALLEGRO_AUDIO_RECORDER> m_recorder;
    };

    
} // namespace LSWE
} // namespace Audio