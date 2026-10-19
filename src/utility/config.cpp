#include <LSWE/utility/config.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Utility {

        Config Config::create() {
            SingletonOf<AllegroInit>::instance().setup();

            return Config(al_create_config());
        }

        Config Config::get_system() {
            SingletonOf<AllegroInit>::instance().setup();

            return Config(al_get_system_config(), false);
        }

        Config Config::load(const char* filename) {
            SingletonOf<AllegroInit>::instance().setup();

            return Config(al_load_config_file(filename));
        }

        Config Config::load(Utility::File file) {
            SingletonOf<AllegroInit>::instance().setup();

            return Config(al_load_config_file_f(file));
        }

        bool Config::save(const char* filename) const {
            return al_save_config_file(filename, m_config.get());
        }

        bool Config::save(Utility::File to_file) const {
            return al_save_config_file_f(to_file, m_config.get());
        }

        Config Config::merge(const Config& cfg1, const Config& cfg2) {
            return al_merge_config(cfg1, cfg2);
        }

        void Config::merge(const Config& overwrite_from) {
            al_merge_config_into(m_config.get(), overwrite_from);
        }

        void Config::destroy() {
            m_config.reset();
        }

        void Config::add_section(const char* section, const char* comment) {
            if (comment) // comment adds section if missing
                al_add_config_comment(m_config.get(), section, comment);
            else
                al_add_config_section(m_config.get(), section);
        }

        void Config::remove_section(const char* section) {
            al_remove_config_section(m_config.get(), section);
        }

        void Config::set(const char* section, const char* key, const char* value) {
            al_set_config_value(m_config.get(), section, key, value);
        }

        const char* Config::get(const char* section, const char* key) const {
            return al_get_config_value(m_config.get(), section, key);
        }

        bool Config::remove(const char* section, const char* key) {
            return al_remove_config_key(m_config.get(), section, key);
        }

        std::vector<std::string> Config::get_all_sections() const {
            ALLEGRO_CONFIG_SECTION* iterator{};
            std::vector<std::string> sections;

            const char* str = al_get_first_config_section(m_config.get(), &iterator);
            
            while(str != nullptr) {
                sections.push_back(str);
                str = al_get_next_config_section(&iterator);
            }
            
            return sections;
        }

        std::vector<std::string> Config::get_all_entries(const char* section) const {
            ALLEGRO_CONFIG_ENTRY* iterator{};
            std::vector<std::string> entries;

            const char* str = al_get_first_config_entry(m_config.get(), section, &iterator);
            
            while(str != nullptr) {
                entries.push_back(str);
                str = al_get_next_config_entry(&iterator);
            }
            
            return entries;
        }

        Config::operator ALLEGRO_CONFIG*() const {
            return m_config.get();
        }

        Config::Config(ALLEGRO_CONFIG* config, bool handle)
            : m_config(config, handle ? al_destroy_config : [](ALLEGRO_CONFIG*){})
        {
            if (!m_config)
                throw Exception::NullException("Config was null");
        }


} // namespace Utility
} // namespace LSWE