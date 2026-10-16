#pragma once

#include <allegro5/allegro5.h>

#include <memory>

namespace LSWE {
namespace Graphics {

    class Shader {
    public:
        static Shader create(ALLEGRO_SHADER_PLATFORM platform = ALLEGRO_SHADER_AUTO);
        static Shader get_current();

        bool attach_source(ALLEGRO_SHADER_TYPE type, const char* source_data);
        bool attach_file(ALLEGRO_SHADER_TYPE type, const char* filename);

        const char* get_log() const;
        ALLEGRO_SHADER_PLATFORM get_platform() const;

        bool build();
        bool use();

        void destroy();

        bool set_sampler(const char* name, ALLEGRO_BITMAP* bitmap, int unit);
        bool set_matrix(const char* name, const ALLEGRO_TRANSFORM* matrix);
        bool set_int(const char* name, int i);
        bool set_float(const char* name, float f);
        bool set_bool(const char* name, bool b);
        bool set_int_vector(const char* name, int num_columns, const int* i, int num_lines);
        bool set_float_vector(const char* name, int num_columns, const float* f, int num_lines);

        static char const* get_default_shader_source(ALLEGRO_SHADER_PLATFORM platform, ALLEGRO_SHADER_TYPE type);

        operator bool() const;
    private:
        Shader(ALLEGRO_SHADER* shader, bool manage_shader);

        bool is_current_this() const;

        std::shared_ptr<ALLEGRO_SHADER> m_shader;
    };
    
} // namespace Graphics
} // namespace LSWE