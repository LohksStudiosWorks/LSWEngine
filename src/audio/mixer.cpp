#include <LSWE/audio/mixer.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/mixer_exception.hpp>

namespace LSWE {
namespace Utility {

    Mixer Mixer::create(unsigned int freq, ALLEGRO_AUDIO_DEPTH depth, ALLEGRO_CHANNEL_CONF chan_conf) {
        SingletonOf<AllegroInit>::instance().setup();
        SingletonOf<AllegroInitAudio>::instance().setup();

        return Mixer(al_create_mixer(freq, depth, chan_conf));
    }

    Mixer Mixer::get_default() {
        SingletonOf<AllegroInit>::instance().setup();
        SingletonOf<AllegroInitAudio>::instance().setup();

        return Mixer();
    }

    void Mixer::set_as_default() {
        al_set_default_mixer(m_mixer.get());
    }

    void Mixer::restore_default() {
        al_restore_default_mixer();
    }

    bool Mixer::reserve_default_samples(int samples) {
        return al_reserve_samples(samples);
    }

    void Mixer::destroy() {
        m_mixer.reset();
    }

    bool Mixer::attach(ALLEGRO_AUDIO_STREAM* audio_stream) {
        return al_attach_audio_stream_to_mixer(audio_stream, m_mixer.get());
    }

    bool Mixer::attach(ALLEGRO_MIXER* mixer) {
        return al_attach_mixer_to_mixer(mixer, m_mixer.get());
    }

    bool Mixer::attach(ALLEGRO_SAMPLE_INSTANCE* sample_instance) {
        return al_attach_sample_instance_to_mixer(sample_instance, m_mixer.get());
    }

    Mixer& Mixer::operator<<(ALLEGRO_AUDIO_STREAM* audio_stream) {
        if (!attach(audio_stream)) 
            throw MixerException("Could not attach audio stream to mixer!");
        return *this;
    }

    Mixer& Mixer::operator<<(ALLEGRO_MIXER* mixer) {
        if (!attach(mixer)) 
            throw MixerException("Could not attach mixer to mixer!");
        return *this;
    }

    Mixer& Mixer::operator<<(ALLEGRO_SAMPLE_INSTANCE* sample_instance) {        
        if (!attach(sample_instance)) 
            throw MixerException("Could not attach sample instance to mixer!");
        return *this;
    }

    unsigned Mixer::get_frequency() const {
        return al_get_mixer_frequency(m_mixer.get());
    }

    ALLEGRO_CHANNEL_CONF Mixer::get_channels() const {
        return al_get_mixer_channels(m_mixer.get());
    }

    ALLEGRO_AUDIO_DEPTH Mixer::get_depth() const {
        return al_get_mixer_depth(m_mixer.get());
    }

    float Mixer::get_gain() const {
        return al_get_mixer_gain(m_mixer.get());
    }

    ALLEGRO_MIXER_QUALITY Mixer::get_quality() const {
        return al_get_mixer_quality(m_mixer.get());
    }

    bool Mixer::get_playing() const {
        return al_get_mixer_playing(m_mixer.get());
    }

    bool Mixer::get_is_attached() const {
        return al_get_mixer_attached(m_mixer.get());
    }

    bool Mixer::get_has_attachments() const {
        return al_mixer_has_attachments(m_mixer.get());
    }

    bool Mixer::set_frequency(unsigned val) {
        return al_set_mixer_frequency(m_mixer.get(), val);
    }

    bool Mixer::set_gain(float val) {
        return al_set_mixer_gain(m_mixer.get(), val);
    }

    bool Mixer::set_quality(ALLEGRO_MIXER_QUALITY val) {
        return al_set_mixer_quality(m_mixer.get(), val);
    }

    bool Mixer::set_playing(bool val) {
        return al_set_mixer_playing(m_mixer.get(), val);
    }

    bool Mixer::set_postprocess_callback(void (*pp_callback)(void *buf, unsigned int samples, void *data), void *pp_callback_userdata) {
        return al_set_mixer_postprocess_callback(m_mixer.get(), pp_callback, pp_callback_userdata);
    }

    Mixer::operator ALLEGRO_MIXER*() {
        return m_mixer.get();
    }

    Mixer::Mixer(ALLEGRO_MIXER*&& mixer) 
        : m_mixer(std::move(mixer), al_destroy_mixer)
    {}

    Mixer::Mixer()
        : m_mixer(al_get_default_mixer(), [](ALLEGRO_MIXER* v){})
    {}
    
} // namespace LSWE
} // namespace Utility