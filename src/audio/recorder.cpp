#include <LSWE/audio/recorder.hpp>

#include <LSWE/utility/startup.hpp>

namespace LSWE {
namespace Utility {

    Recorder Recorder::create(size_t fragment_count, unsigned int frag_samples, unsigned int freq, ALLEGRO_AUDIO_DEPTH depth, ALLEGRO_CHANNEL_CONF conf) {
        return Recorder(al_create_audio_recorder(fragment_count, frag_samples, freq, depth, conf));
    }
    
    ALLEGRO_AUDIO_RECORDER_EVENT* Recorder::get_from_event(ALLEGRO_EVENT* event) {
        return al_get_audio_recorder_event(event);
    }

    void Recorder::destroy() {
        m_recorder.reset();
    }

    bool Recorder::start() {
        return al_start_audio_recorder(m_recorder.get());
    }

    void Recorder::stop() {
        al_stop_audio_recorder(m_recorder.get());
    }

    bool Recorder::get_is_recording() const {
        return al_is_audio_recorder_recording(m_recorder.get());
    }

    ALLEGRO_EVENT_SOURCE* Recorder::get_event_source() const {
        return al_get_audio_recorder_event_source(m_recorder.get());
    }

    Recorder::operator ALLEGRO_EVENT_SOURCE*() const {
        return al_get_audio_recorder_event_source(m_recorder.get());
    }

    Recorder::operator ALLEGRO_AUDIO_RECORDER*() {
        return m_recorder.get();
    }

    Recorder::Recorder(ALLEGRO_AUDIO_RECORDER*&& recorder)
        : m_recorder(std::move(recorder), al_destroy_audio_recorder)
    {}
    
} // namespace LSWE
} // namespace Utility