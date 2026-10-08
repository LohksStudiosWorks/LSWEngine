#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_audio.h>

#include <memory>

namespace LSWE {
namespace Audio {

    /**
     * @brief Voice is a representation of a device, generally, like your headphones.
     * 
     * You can attach mixers of other audio instances / streams to this and play directly on the sound output.
     */
    class Voice {
    public:
        static Voice create(unsigned int freq = 48000, ALLEGRO_AUDIO_DEPTH depth = ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF chan_conf = ALLEGRO_CHANNEL_CONF_2);
        static Voice get_default();

        void set_as_default();
        static void unset_default();

        void destroy();
        void detach();

        bool attach(ALLEGRO_AUDIO_STREAM* audio_stream);
        bool attach(ALLEGRO_MIXER* mixer);
        bool attach(ALLEGRO_SAMPLE_INSTANCE* sample_instance);

        Voice& operator<<(ALLEGRO_AUDIO_STREAM* audio_stream);
        Voice& operator<<(ALLEGRO_MIXER* mixer);
        Voice& operator<<(ALLEGRO_SAMPLE_INSTANCE* sample_instance);

        unsigned get_frequency() const;
        ALLEGRO_CHANNEL_CONF get_channels() const;
        ALLEGRO_AUDIO_DEPTH get_depth() const;
        bool get_playing() const;
        unsigned get_position() const;

        bool set_playing(bool playing);
        bool set_position(unsigned position);

        operator ALLEGRO_VOICE*() const;
    private:
        Voice(ALLEGRO_VOICE* voice);
        Voice();

        std::shared_ptr<ALLEGRO_VOICE> m_voice;
    };

} // namespace Audio
} // namespace LSWE