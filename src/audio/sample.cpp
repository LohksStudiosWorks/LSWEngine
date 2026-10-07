#include <LSWE/audio/sample.hpp>

#include <LSWE/utility/startup.hpp>

namespace LSWE {
namespace Utility {

    std::optional<ALLEGRO_SAMPLE_ID> Sample::play(float gain, float pan, float speed, ALLEGRO_PLAYMODE loop) {
        ALLEGRO_SAMPLE_ID id;
        if (al_play_sample(m_sample.get(), gain, pan, speed, loop, &id))
            return id;
        return std::nullopt;
    }

    void Sample::stop(ALLEGRO_SAMPLE_ID sample_id) {
        al_stop_sample(&sample_id);
    }

    Sample Sample::create(void* buf, unsigned samples, unsigned freq, ALLEGRO_AUDIO_DEPTH depth, ALLEGRO_CHANNEL_CONF conf, bool free_buf) {
        SingletonOf<AllegroInit>::instance().setup();
        SingletonOf<AllegroInitAudio>::instance().setup();

        return Sample(al_create_sample(buf, samples, freq, depth, conf, free_buf));
    }

    Sample Sample::load(const char* path) {
        SingletonOf<AllegroInit>::instance().setup();
        SingletonOf<AllegroInitAudio>::instance().setup();

        return Sample(al_load_sample(path));
    }

    Sample Sample::load(ALLEGRO_FILE* file, const char* ident) {
        SingletonOf<AllegroInit>::instance().setup();
        SingletonOf<AllegroInitAudio>::instance().setup();

        return Sample(al_load_sample_f(file, ident));
    }

    bool Sample::save(const char* filename) {
        return al_save_sample(filename, m_sample.get());
    }

    bool Sample::save(ALLEGRO_FILE* file, const char* ident) {
        return al_save_sample_f(file, ident, m_sample.get());
    }

    void Sample::destroy() {
        m_sample.reset();
    }    

    unsigned Sample::get_frequency() const {
        return al_get_sample_frequency(m_sample.get());
    }

    ALLEGRO_CHANNEL_CONF Sample::get_channels() const {
        return al_get_sample_channels(m_sample.get());
    }

    ALLEGRO_AUDIO_DEPTH Sample::get_depth() const {
        return al_get_sample_depth(m_sample.get());
    }

    unsigned Sample::get_length() const {
        return al_get_sample_length(m_sample.get());
    }

    void* Sample::get_data() const {
        return al_get_sample_data(m_sample.get());
    }    

    Sample::operator ALLEGRO_SAMPLE*() const {
        return m_sample.get();
    }

    Sample::Sample(ALLEGRO_SAMPLE*&& sample)
        : m_sample(std::move(sample), al_destroy_sample)
    {}
    
} // namespace LSWE
} // namespace Utility