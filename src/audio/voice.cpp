#include <LSWE/audio/voice.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/voice_exception.hpp>

namespace LSWE {
namespace Audio {

    Voice Voice::create(unsigned int freq, ALLEGRO_AUDIO_DEPTH depth, ALLEGRO_CHANNEL_CONF chan_conf) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitAudio>::instance().setup();

        return Voice(al_create_voice(freq, depth, chan_conf));
    }

    Voice Voice::get_default() {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitAudio>::instance().setup();

        return Voice();
    }

    void Voice::set_as_default() {
        al_set_default_voice(m_voice.get());
    }

    void Voice::unset_default() {
        al_set_default_voice(nullptr);
    }

    void Voice::destroy() {
        m_voice.reset();
    }

    void Voice::detach() {
        al_detach_voice(m_voice.get());
    }

    bool Voice::attach(ALLEGRO_AUDIO_STREAM* audio_stream) {
        return al_attach_audio_stream_to_voice(audio_stream, m_voice.get());
    }

    bool Voice::attach(ALLEGRO_MIXER* mixer) {
        return al_attach_mixer_to_voice(mixer, m_voice.get());
    }

    bool Voice::attach(ALLEGRO_SAMPLE_INSTANCE* sample_instance) {
        return al_attach_sample_instance_to_voice(sample_instance, m_voice.get());
    }

    Voice& Voice::operator<<(ALLEGRO_AUDIO_STREAM* audio_stream) {
        if (!attach(audio_stream)) 
            throw Utility::VoiceException("Could not attach audio stream to voice!");
        return *this;
    }

    Voice& Voice::operator<<(ALLEGRO_MIXER* mixer) {
        if (!attach(mixer)) 
            throw Utility::VoiceException("Could not attach mixer to voice!");
        return *this;
    }

    Voice& Voice::operator<<(ALLEGRO_SAMPLE_INSTANCE* sample_instance) {        
        if (!attach(sample_instance)) 
            throw Utility::VoiceException("Could not attach sample instance to voice!");
        return *this;
    }

    unsigned Voice::get_frequency() const {
        return al_get_voice_frequency(m_voice.get());
    }

    ALLEGRO_CHANNEL_CONF Voice::get_channels() const {
        return al_get_voice_channels(m_voice.get());
    }

    ALLEGRO_AUDIO_DEPTH Voice::get_depth() const {
        return al_get_voice_depth(m_voice.get());
    }

    bool Voice::get_playing() const {
        return al_get_voice_playing(m_voice.get());
    }

    unsigned Voice::get_position() const {
        return al_get_voice_position(m_voice.get());
    }

    bool Voice::set_playing(bool playing) {
        return al_set_voice_playing(m_voice.get(), playing);
    }

    bool Voice::set_position(unsigned position) {
        return al_set_voice_position(m_voice.get(), position);
    }

    Voice::operator ALLEGRO_VOICE*()  const {
        return m_voice.get();
    }

    Voice::Voice(ALLEGRO_VOICE*&& voice) 
        : m_voice(std::move(voice), al_destroy_voice)
    {}

    Voice::Voice()
        : m_voice(al_get_default_voice(), [](ALLEGRO_VOICE* v){})
    {}
    
} // namespace LSWE
} // namespace Audio