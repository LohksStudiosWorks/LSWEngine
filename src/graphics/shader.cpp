#include <LSWE/graphics/shader.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Graphics {

    Shader Shader::create(ALLEGRO_SHADER_PLATFORM platform) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();

        return Shader(al_create_shader(platform), true);
    }

    Shader Shader::get_current() {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();

        return Shader(al_get_current_shader(), false);
    }

    bool Shader::attach_source(ALLEGRO_SHADER_TYPE type, const char* source_data) {
        return al_attach_shader_source(m_shader.get(), type, source_data);
    }

    bool Shader::attach_file(ALLEGRO_SHADER_TYPE type, const char* filename) {
        return al_attach_shader_source_file(m_shader.get(), type, filename);
    }
        
    const char* Shader::get_log() const {
        return al_get_shader_log(m_shader.get());
    }

    ALLEGRO_SHADER_PLATFORM Shader::get_platform() const {
        return al_get_shader_platform(m_shader.get());
    }

    bool Shader::build() {
        return al_build_shader(m_shader.get());
    }

    bool Shader::use() {
        return al_use_shader(m_shader.get());
    }

    void Shader::destroy() {
        m_shader.reset();
    }

    bool Shader::set_sampler(const char* name, ALLEGRO_BITMAP* bitmap, int unit) {
        if (!is_current_this()) return false;
        return al_set_shader_sampler(name, bitmap, unit);
    }

    bool Shader::set_matrix(const char* name, const ALLEGRO_TRANSFORM* matrix) {
        if (!is_current_this()) return false;
        return al_set_shader_matrix(name, matrix);
    }

    bool Shader::set_int(const char* name, int i) {
        if (!is_current_this()) return false;
        return al_set_shader_int(name, i);
    }

    bool Shader::set_float(const char* name, float f) {
        if (!is_current_this()) return false;
        return al_set_shader_float(name, f);
    }

    bool Shader::set_bool(const char* name, bool b) {
        if (!is_current_this()) return false;
        return al_set_shader_bool(name, b);
    }

    bool Shader::set_int_vector(const char* name, int num_columns, const int* i, int num_lines) {
        if (!is_current_this() || num_columns < 1 || num_columns > 4) return false;
        return al_set_shader_int_vector(name, num_columns, i, num_lines);
    }

    bool Shader::set_float_vector(const char* name, int num_columns, const float* f, int num_lines) {
        if (!is_current_this() || num_columns < 1 || num_columns > 4) return false;
        return al_set_shader_float_vector(name, num_columns, f, num_lines);
    }

    char const* Shader::get_default_shader_source(ALLEGRO_SHADER_PLATFORM platform, ALLEGRO_SHADER_TYPE type) {
        return al_get_default_shader_source(platform, type);
    }

    Shader::operator bool() const {
        return m_shader.get() != nullptr;
    }

    Shader::Shader(ALLEGRO_SHADER* shader, bool manage_shader) 
        : m_shader(shader, manage_shader ? al_destroy_shader : [](ALLEGRO_SHADER*){})
    {
        if (!m_shader)
            throw Exception::NullException("Shader was null");
    }

    bool Shader::is_current_this() const {
        return al_get_current_shader() == m_shader.get();
    }
    
} // namespace Graphics
} // namespace LSWE