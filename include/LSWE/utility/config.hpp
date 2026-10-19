#pragma once

#include <allegro5/allegro5.h>

#include <memory>
#include <vector>
#include <string>

#include <LSWE/utility/file.hpp>

namespace LSWE {
namespace Utility {

    class Config {
    public:
        static Config create();
        static Config get_system();
        static Config load(const char* filename);
        static Config load(Utility::File file);
        
        bool save(const char* filename) const;
        bool save(Utility::File to_file) const;

        static Config merge(const Config& cfg1, const Config& cfg2);
        void merge(const Config& overwrite_from);

        void destroy();

        void add_section(const char* section, const char* comment = nullptr);
        void remove_section(const char* section);

        void set(const char* section, const char* key, const char* value);
        const char* get(const char* section, const char* key) const;
        bool remove(const char* section, const char* key);

        std::vector<std::string> get_all_sections() const;
        std::vector<std::string> get_all_entries(const char* section) const;

        operator ALLEGRO_CONFIG*() const;
    private:
        Config(ALLEGRO_CONFIG* config, bool handle = true);

        std::shared_ptr<ALLEGRO_CONFIG> m_config;
    };

} // namespace Utility
} // namespace LSWE