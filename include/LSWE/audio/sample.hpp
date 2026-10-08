#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_audio.h>

#include <optional>

#include <memory>

namespace LSWE {
namespace Audio {

    class SampleInstance;

    class Sample {
    public:
        std::optional<ALLEGRO_SAMPLE_ID> play(float gain = 1.0f, float pan = 0.0f, float speed = 1.0f, ALLEGRO_PLAYMODE loop = ALLEGRO_PLAYMODE_ONCE);
        static void stop(ALLEGRO_SAMPLE_ID sample_id);

        static Sample create(void* buf, unsigned samples, unsigned freq, ALLEGRO_AUDIO_DEPTH depth, ALLEGRO_CHANNEL_CONF conf, bool free_buf = false);
        static Sample load(const char* path);
        static Sample load(ALLEGRO_FILE* file, const char* ident = nullptr);

        bool save(const char* filename);
        bool save(ALLEGRO_FILE* file, const char* ident = ".wav");
        void destroy();

        unsigned get_frequency() const;
        ALLEGRO_CHANNEL_CONF get_channels() const;
        ALLEGRO_AUDIO_DEPTH get_depth() const;
        unsigned get_length() const;
        void* get_data() const;
        
        operator ALLEGRO_SAMPLE*() const;
    private:
        Sample(ALLEGRO_SAMPLE* sample);

        friend class SampleInstance;

        std::shared_ptr<ALLEGRO_SAMPLE> m_sample;
    };

} // namespace Audio
} // namespace LSWE