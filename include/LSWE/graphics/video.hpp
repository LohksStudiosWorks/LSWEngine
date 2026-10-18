#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_video.h>

#include <memory>

#include <LSWE/utility/file.hpp>
#include <LSWE/audio/mixer.hpp>
#include <LSWE/audio/voice.hpp>

#include <LSWE/graphics/bitmap.hpp>

namespace LSWE {
namespace Graphics {

    class Video : public DrawableBitmap {
    public:
        static Video open(const char* filename);
        static Video open(Utility::File&& file, const char* ident = nullptr);

        void start(const Audio::Mixer& on_mixer);
        void start(const Audio::Voice& on_voice);

        bool get_playing() const;
        double get_audio_rate() const;
        double get_fps() const;
        float get_width_scaled() const;
        float get_height_scaled() const;
        double get_time_seconds(ALLEGRO_VIDEO_POSITION_TYPE type = ALLEGRO_VIDEO_POSITION_ACTUAL) const;

        ALLEGRO_BITMAP* get_current_frame() const;

        void set_playing(bool play);
        bool seek_seconds(double secs);

        ALLEGRO_EVENT_SOURCE* get_event_source() const;

        operator ALLEGRO_EVENT_SOURCE*() const;
        operator ALLEGRO_VIDEO*() const;
    private:
        Video(ALLEGRO_VIDEO* video);

        std::shared_ptr<ALLEGRO_VIDEO> m_video;

        ALLEGRO_BITMAP* get_bitmap() const override;
    };
    
} // namespace Graphics
} // namespace LSWE