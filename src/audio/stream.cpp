#include <LSWE/audio/stream.hpp>

#include <LSWE/utility/startup.hpp>

namespace LSWE {
namespace Utility {

    Stream Stream::create(size_t fragment_count, unsigned int frag_samples, unsigned int freq, ALLEGRO_AUDIO_DEPTH depth, ALLEGRO_CHANNEL_CONF conf) {
        SingletonOf<AllegroInit>::instance().setup();
        SingletonOf<AllegroInitAudio>::instance().setup();

        return Stream(al_create_audio_stream(fragment_count, frag_samples, freq, depth, conf));
    }

    Stream Stream::load(const char* filename, size_t buffer_count, unsigned samples) {
        SingletonOf<AllegroInit>::instance().setup();
        SingletonOf<AllegroInitAudio>::instance().setup();

        return Stream(al_load_audio_stream(filename, buffer_count, samples));
    }

    Stream Stream::load(ALLEGRO_FILE* file, const char* ident, size_t buffer_count, unsigned samples) {
        SingletonOf<AllegroInit>::instance().setup();
        SingletonOf<AllegroInitAudio>::instance().setup();

        return Stream(al_load_audio_stream_f(file, ident, buffer_count, samples));
    }

    void Stream::destroy() {
        m_stream.reset();
    }

    void Stream::drain() {
        al_drain_audio_stream(m_stream.get());
    }

    bool Stream::rewind() {
        return al_rewind_audio_stream(m_stream.get());
    }

    bool Stream::seek_seconds(double time) {
        return al_seek_audio_stream_secs(m_stream.get(), time);
    }

    ALLEGRO_EVENT_SOURCE* Stream::get_event_source() const {
        return al_get_audio_stream_event_source(m_stream.get());
    }

    unsigned Stream::get_frequency() const {
        return al_get_audio_stream_frequency(m_stream.get());
    }

    ALLEGRO_CHANNEL_CONF Stream::get_channels() const {
        return al_get_audio_stream_channels(m_stream.get());
    }

    ALLEGRO_AUDIO_DEPTH Stream::get_depth() const {
        return al_get_audio_stream_depth(m_stream.get());
    }

    unsigned Stream::get_length() const {
        return al_get_audio_stream_length(m_stream.get());
    }

    float Stream::get_speed() const {
        return al_get_audio_stream_speed(m_stream.get());
    }

    float Stream::get_gain() const {
        return al_get_audio_stream_gain(m_stream.get());
    }

    float Stream::get_pan() const {
        return al_get_audio_stream_pan(m_stream.get());
    }

    ALLEGRO_PLAYMODE Stream::get_playmode() const {
        return al_get_audio_stream_playmode(m_stream.get());
    }

    bool Stream::get_playing() const {
        return al_get_audio_stream_playing(m_stream.get());
    }

    bool Stream::get_is_attached() const {
        return al_get_audio_stream_attached(m_stream.get());
    }

    uint64_t Stream::get_played_samples() const {
        return al_get_audio_stream_played_samples(m_stream.get());
    }

    unsigned Stream::get_fragments() const {
        return al_get_audio_stream_fragments(m_stream.get());
    }

    unsigned Stream::get_available_fragments() const {
        return al_get_available_audio_stream_fragments(m_stream.get());
    }

    double Stream::get_position_seconds() const {
        return al_get_audio_stream_position_secs(m_stream.get());
    }

    double Stream::get_length_seconds() const {
        return al_get_audio_stream_length_secs(m_stream.get());
    }

    bool Stream::set_speed(float val) {
        return al_set_audio_stream_speed(m_stream.get(), val);
    }

    bool Stream::set_gain(float val) {
        return al_set_audio_stream_gain(m_stream.get(), val);
    }

    bool Stream::set_pan(float val) {
        return al_set_audio_stream_pan(m_stream.get(), val);
    }

    bool Stream::set_playing(bool val) {
        return al_set_audio_stream_playing(m_stream.get(), val);
    }

    bool Stream::set_playmode(ALLEGRO_PLAYMODE val) {
        return al_set_audio_stream_playmode(m_stream.get(), val);
    }

    bool Stream::set_loop_seconds(double start, double end) {
        return al_set_audio_stream_loop_secs(m_stream.get(), start, end);
    }

    void* Stream::get_fragment() {
        return al_get_audio_stream_fragment(m_stream.get());
    }

    bool Stream::set_fragment(void* val) {
        return al_set_audio_stream_fragment(m_stream.get(), val);
    }

    bool Stream::detach(){
        return al_detach_audio_stream(m_stream.get());
    }

    Stream::operator ALLEGRO_EVENT_SOURCE*() const {
        return al_get_audio_stream_event_source(m_stream.get());
    }

    Stream::operator ALLEGRO_AUDIO_STREAM*() {
        return m_stream.get();
    }

    Stream::Stream(ALLEGRO_AUDIO_STREAM*&& stream) 
        : m_stream(std::move(stream), al_destroy_audio_stream)
    {}

    
} // namespace LSWE
} // namespace Utility