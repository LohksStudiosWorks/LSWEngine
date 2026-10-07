#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_audio.h>

#include <LSWE/utility/memory.hpp>

namespace LSWE {
namespace Utility {

    class Stream {
    public:
        static Stream create(size_t fragment_count, unsigned int frag_samples, unsigned int freq = 48000, ALLEGRO_AUDIO_DEPTH depth = ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF conf = ALLEGRO_CHANNEL_CONF_2);
        static Stream load(const char* filename, size_t buffer_count = 4, unsigned samples = 2048);
        static Stream load(ALLEGRO_FILE* file, const char* ident = ".wav", size_t buffer_count = 4, unsigned samples = 2048);

        void destroy();
        void drain();
        bool rewind();
        bool seek_seconds(double time);

        ALLEGRO_EVENT_SOURCE* get_event_source() const;
        
        unsigned get_frequency() const;
        ALLEGRO_CHANNEL_CONF get_channels() const;
        ALLEGRO_AUDIO_DEPTH get_depth() const;
        unsigned get_length() const;
        float get_speed() const;
        float get_gain() const;
        float get_pan() const;
        ALLEGRO_PLAYMODE get_playmode() const;
        bool get_playing() const;
        bool get_is_attached() const;
        uint64_t get_played_samples() const;
        unsigned get_fragments() const;
        unsigned get_available_fragments() const;
        double get_position_seconds() const;
        double get_length_seconds() const;

        bool set_speed(float val);
        bool set_gain(float val);
        bool set_pan(float val);
        bool set_playing(bool val);
        bool set_playmode(ALLEGRO_PLAYMODE val);
        bool set_loop_seconds(double start, double end);

        void* get_fragment();
        bool set_fragment(void* val);

        bool detach();

        operator ALLEGRO_EVENT_SOURCE*() const;
        operator ALLEGRO_AUDIO_STREAM*();
    private:
        Stream(ALLEGRO_AUDIO_STREAM*&& stream);

        LazyPointer<ALLEGRO_AUDIO_STREAM> m_stream;
    };

    /*class Stream {
    public:
        Stream(const std::string& path);
        Stream(Stream&& oth) noexcept;

        void operator=(Stream&& oth);

        
        
        bool valid() const;
        operator bool() const;
    private:        
        LazyPointer<ALLEGRO_AUDIO_STREAM> m_stream;
    };*/ 

    
} // namespace LSWE
} // namespace Utility