#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_audio.h>

#include <memory>

#include <LSWE/audio/sample.hpp>

namespace LSWE {
namespace Audio {

    class SampleInstance {
    public:
        static SampleInstance create(const Sample& sample);
        void destroy();

        bool play();
        bool stop();

        unsigned get_frequency() const;
        ALLEGRO_CHANNEL_CONF get_channels() const;
        ALLEGRO_AUDIO_DEPTH get_depth() const;
        unsigned get_length() const;
        unsigned get_position() const;
        float get_speed() const;
        float get_gain() const;
        float get_pan() const;
        float get_time_seconds() const;
        ALLEGRO_PLAYMODE get_playmode() const;
        bool get_playing() const;
        bool get_is_attached() const;

        bool set_length(unsigned val);
        bool set_position(unsigned val);
        bool set_speed(float val);
        bool set_gain(float val);
        bool set_pan(float val);
        bool set_playmode(ALLEGRO_PLAYMODE val);
        bool set_playing(bool val);
        
        bool detach();
        
        operator ALLEGRO_SAMPLE_INSTANCE*() const;
    private:
        SampleInstance(ALLEGRO_SAMPLE_INSTANCE* sample_instance);

        std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> m_instance;
    };

    // Backward compatibility, at least a bit. Track was SampleInstance.
    using Track = SampleInstance;

    
} // namespace LSWE
} // namespace Audio