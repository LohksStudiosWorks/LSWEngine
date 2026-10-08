#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_audio.h>

#include <memory>

namespace LSWE {
namespace Audio {

    class Mixer {
    public:
        static Mixer create(unsigned int freq = 48000, ALLEGRO_AUDIO_DEPTH depth = ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF chan_conf = ALLEGRO_CHANNEL_CONF_2);
        static Mixer get_default();

        void set_as_default();
        static bool restore_default();

        static bool reserve_default_samples(int samples);

        void destroy();

        bool attach(ALLEGRO_AUDIO_STREAM* audio_stream);
        bool attach(ALLEGRO_MIXER* mixer);
        bool attach(ALLEGRO_SAMPLE_INSTANCE* sample_instance);

        Mixer& operator<<(ALLEGRO_AUDIO_STREAM* audio_stream);
        Mixer& operator<<(ALLEGRO_MIXER* mixer);
        Mixer& operator<<(ALLEGRO_SAMPLE_INSTANCE* sample_instance);

        unsigned get_frequency() const;
        ALLEGRO_CHANNEL_CONF get_channels() const;
        ALLEGRO_AUDIO_DEPTH get_depth() const;
        float get_gain() const;
        ALLEGRO_MIXER_QUALITY get_quality() const;
        bool get_playing() const;
        bool get_is_attached() const;
        bool get_has_attachments() const;

        bool set_frequency(unsigned val);
        bool set_gain(float val);
        bool set_quality(ALLEGRO_MIXER_QUALITY val);
        bool set_playing(bool val);

        bool set_postprocess_callback(void (*pp_callback)(void *buf, unsigned int samples, void *data), void *pp_callback_userdata);
        
        operator ALLEGRO_MIXER*() const;
    private:
        Mixer(ALLEGRO_MIXER* mixer);
        Mixer();

        std::shared_ptr<ALLEGRO_MIXER> m_mixer;
    };

    
} // namespace LSWE
} // namespace Audio