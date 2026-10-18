#include <LSWE/graphics/video.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Graphics {
    
    Video Video::open(const char* filename) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitVideo>::instance().setup();

        return Video(al_open_video(filename));
    }

    Video Video::open(Utility::File&& file, const char* ident) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitVideo>::instance().setup();

        ALLEGRO_FILE* released = file.release();

        return Video(al_open_video_f(released, ident));
    }

    void Video::start(const Audio::Mixer& on_mixer) {
        al_start_video(m_video.get(), on_mixer);
    }

    void Video::start(const Audio::Voice& on_voice) {
        al_start_video_with_voice(m_video.get(), on_voice);
    }

    bool Video::get_playing() const {
        return al_is_video_playing(m_video.get());
    }

    double Video::get_audio_rate() const {
        return al_get_video_audio_rate(m_video.get());
    }

    double Video::get_fps() const {
        return al_get_video_fps(m_video.get());
    }

    float Video::get_width_scaled() const {
        return al_get_video_scaled_width(m_video.get());
    }

    float Video::get_height_scaled() const {
        return al_get_video_scaled_height(m_video.get());
    }

    double Video::get_time_seconds(ALLEGRO_VIDEO_POSITION_TYPE type) const {
        return al_get_video_position(m_video.get(), type);
    }

    ALLEGRO_BITMAP* Video::get_current_frame() const {
        return al_get_video_frame(m_video.get());
    }

    void Video::set_playing(bool play) {
        return al_set_video_playing(m_video.get(), play);
    }

    bool Video::seek_seconds(double secs) {
        return al_seek_video(m_video.get(), secs);
    }

    ALLEGRO_EVENT_SOURCE* Video::get_event_source() const {
        return al_get_video_event_source(m_video.get());
    }

    Video::operator ALLEGRO_EVENT_SOURCE*() const {
        return al_get_video_event_source(m_video.get());
    }

    Video::operator ALLEGRO_VIDEO*() const {
        return m_video.get();
    }

    Video::Video(ALLEGRO_VIDEO* video)
        : m_video(video, al_close_video)
    {
        if (!m_video)
            throw Exception::NullException("Video was null");
    }

    ALLEGRO_BITMAP* Video::get_bitmap() const {
        return get_current_frame();
    }

} // namespace Graphics
} // namespace LSWE