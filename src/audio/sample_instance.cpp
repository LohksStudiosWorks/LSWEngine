#include <LSWE/audio/sample_instance.hpp>

#include <LSWE/utility/startup.hpp>

namespace LSWE {
namespace Audio {

    SampleInstance SampleInstance::create(const Sample& sample) {
        return SampleInstance(al_create_sample_instance(sample));
    }

    void SampleInstance::destroy() {
        m_instance.reset();
    }

    bool SampleInstance::play() {
        return al_play_sample_instance(m_instance.get());
    }

    bool SampleInstance::stop() {
        return al_stop_sample_instance(m_instance.get());
    }

    unsigned SampleInstance::get_frequency() const {
        return al_get_sample_instance_frequency(m_instance.get());
    }

    ALLEGRO_CHANNEL_CONF SampleInstance::get_channels() const {
        return al_get_sample_instance_channels(m_instance.get());
    }

    ALLEGRO_AUDIO_DEPTH SampleInstance::get_depth() const {
        return al_get_sample_instance_depth(m_instance.get());
    }

    unsigned SampleInstance::get_length() const {
        return al_get_sample_instance_length(m_instance.get());
    }

    unsigned SampleInstance::get_position() const {
        return al_get_sample_instance_position(m_instance.get());
    }

    float SampleInstance::get_speed() const {
        return al_get_sample_instance_speed(m_instance.get());
    }

    float SampleInstance::get_gain() const {
        return al_get_sample_instance_gain(m_instance.get());
    }

    float SampleInstance::get_pan() const {
        return al_get_sample_instance_pan(m_instance.get());
    }

    float SampleInstance::get_time_seconds() const {
        return al_get_sample_instance_time(m_instance.get());
    }

    ALLEGRO_PLAYMODE SampleInstance::get_playmode() const {
        return al_get_sample_instance_playmode(m_instance.get());
    }

    bool SampleInstance::get_playing() const {
        return al_get_sample_instance_playing(m_instance.get());
    }

    bool SampleInstance::get_is_attached() const {
        return al_get_sample_instance_attached(m_instance.get());
    }

    bool SampleInstance::set_length(unsigned val) {
        return al_set_sample_instance_length(m_instance.get(), val);
    }

    bool SampleInstance::set_position(unsigned val) {
        return al_set_sample_instance_position(m_instance.get(), val);
    }

    bool SampleInstance::set_speed(float val) {
        return al_set_sample_instance_speed(m_instance.get(), val);
    }

    bool SampleInstance::set_gain(float val) {
        return al_set_sample_instance_gain(m_instance.get(), val);
    }

    bool SampleInstance::set_pan(float val) {
        return al_set_sample_instance_pan(m_instance.get(), val);
    }

    bool SampleInstance::set_playmode(ALLEGRO_PLAYMODE val) {
        return al_set_sample_instance_playmode(m_instance.get(), val);
    }

    bool SampleInstance::set_playing(bool val) {
        return al_set_sample_instance_playing(m_instance.get(), val);
    }

    bool SampleInstance::detach() {
        return al_detach_sample_instance(m_instance.get());
    }

    SampleInstance::operator ALLEGRO_SAMPLE_INSTANCE*() const {
        return m_instance.get();
    }

    SampleInstance::SampleInstance(ALLEGRO_SAMPLE_INSTANCE* sample_instance) 
        : m_instance(sample_instance, al_destroy_sample_instance)
    {}
    
} // namespace LSWE
} // namespace Audio