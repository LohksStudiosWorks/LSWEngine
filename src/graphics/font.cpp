#include <LSWE/graphics/font.hpp>

#include <allegro5/allegro_ttf.h>

#include <stdarg.h>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Graphics {

    namespace detail {
        // note: call va_start(args, fmt); before this
        std::unique_ptr<char[]> handle_variadic_args(const char* fmt, va_list args) {
            va_list cpy;
            va_copy(cpy, args);

            int size = vsnprintf(nullptr, 0, fmt, args);
            if (size <= 0) return {};

            va_end(args);

            auto buf = std::unique_ptr<char[]>(new char[size + 1]);
            vsnprintf(buf.get(), size + 1, fmt, cpy);

            va_end(cpy);

            return buf;
        }
    }

    Font Font::load(const char* filename, int size, int flags) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitFont>::instance().setup();

        return Font(al_load_font(filename, size, flags));
    }

    Font Font::load_ttf(const char* filename, int size, int flags) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitFont>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitTTF>::instance().setup();

        return Font(al_load_ttf_font(filename, size, flags));
    }

    Font Font::load_ttf(Utility::File&& file, int size, int flags) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitFont>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitTTF>::instance().setup();

        const std::string file_path = file.get_path();
        ALLEGRO_FILE* released = file.release();

        return Font(al_load_ttf_font_f(released, file_path.c_str(), size, flags));
    }

    Font Font::load_ttf_stretch(const char* filename, int w, int h, int flags) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitFont>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitTTF>::instance().setup();

        return Font(al_load_ttf_font_stretch(filename, w, h, flags));
    }

    Font Font::load_ttf_stretch(Utility::File&& file, int w, int h, int flags) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitFont>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitTTF>::instance().setup();

        const std::string file_path = file.get_path();
        ALLEGRO_FILE* released = file.release();

        return Font(al_load_ttf_font_stretch_f(released, file_path.c_str(), w, h, flags));
    }

    Font Font::load_from_bitmap(const char* filename, int flags) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitFont>::instance().setup();

        return Font(al_load_bitmap_font_flags(filename, flags));
    }

    Font Font::create_builtin() {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitFont>::instance().setup();

        return Font(al_create_builtin_font());
    }

    Font Font::grab_from_bitmap(ALLEGRO_BITMAP* to_copy, int ranges_n, const int ranges[]) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitFont>::instance().setup();
        
        return Font(al_grab_font_from_bitmap(to_copy, ranges_n, ranges));
    }

    Font Font::grab_from_bitmap(ALLEGRO_BITMAP* to_copy, ranges ranges) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitFont>::instance().setup();

        auto conv = std::unique_ptr<int[]>(new int[ranges.size() * 2]);
        for(size_t p = 0; p < ranges.size(); ++p) {
            conv[p * 2] = ranges[p].first;
            conv[p * 2 + 1] = ranges[p].second;
        }
        
        return Font(al_grab_font_from_bitmap(to_copy, ranges.size(), conv.get()));
    }

    void Font::destroy() {
        m_font.reset();
    }

    int Font::get_line_height() const {
        return al_get_font_line_height(m_font.get());
    }

    int Font::get_ascent() const {
        return al_get_font_ascent(m_font.get());
    }

    int Font::get_descent() const {
        return al_get_font_descent(m_font.get());
    }

    int Font::get_width(const char* str) const {
        return al_get_text_width(m_font.get(), str);
    }

    int Font::get_width(const ALLEGRO_USTR* str) const {
        return al_get_ustr_width(m_font.get(), str);
    }

    void Font::draw(ALLEGRO_COLOR color, float x, float y, int flags, const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
        auto buf = detail::handle_variadic_args(fmt, args);

		al_draw_text(m_font.get(), color, x, y, flags, buf.get());
    }

    void Font::draw(ALLEGRO_COLOR color, float x, float y, int flags, const ALLEGRO_USTR* str) {
        al_draw_ustr(m_font.get(), color, x, y, flags, str);
    }

    void Font::draw_justified(ALLEGRO_COLOR color, float x1, float x2, float y, float diff, int flags, const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
        auto buf = detail::handle_variadic_args(fmt, args);

		al_draw_justified_text(m_font.get(), color, x1, x2, y, diff, flags, buf.get());
    }

    void Font::draw_justified(ALLEGRO_COLOR color, float x1, float x2, float y, float diff, int flags, const ALLEGRO_USTR* str) {
        al_draw_justified_ustr(m_font.get(), color, x1, x2, y, diff, flags, str);
    }

    void Font::draw_multiline(ALLEGRO_COLOR color, float x, float y, float max_width, float line_height, int flags, const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
        auto buf = detail::handle_variadic_args(fmt, args);

		al_draw_multiline_text(m_font.get(), color, x, y, max_width, line_height, flags, buf.get());
    }

    void Font::draw_multiline(ALLEGRO_COLOR color, float x, float y, float max_width, float line_height, int flags, const ALLEGRO_USTR* str) {
		al_draw_multiline_ustr(m_font.get(), color, x, y, max_width, line_height, flags, str);
    }

    void Font::draw_glyph(ALLEGRO_COLOR color, float x, float y, int codepoint) {
        al_draw_glyph(m_font.get(), color, x, y, codepoint);
    }

    void Font::do_multiline(float max_width, bool(*callback)(int line_num, const char* line, int size, void* extra), void* extra, const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
        auto buf = detail::handle_variadic_args(fmt, args);

        al_do_multiline_text(m_font.get(), max_width, buf.get(), callback, extra);
    }

    void Font::do_multiline(float max_width, bool(*callback)(int line_num, const ALLEGRO_USTR *line, void *extra), void* extra, const ALLEGRO_USTR* str) {
        al_do_multiline_ustr(m_font.get(), max_width, str, callback, extra);
    }

    void Font::get_dimensions(const char* str, int& bbx, int& bby, int& bbw, int& bbh) const {
        al_get_text_dimensions(m_font.get(), str, &bbx, &bby, &bbw, &bbh);
    }

    void Font::get_dimensions(const ALLEGRO_USTR* str, int& bbx, int& bby, int& bbw, int& bbh) const {
        al_get_ustr_dimensions(m_font.get(), str, &bbx, &bby, &bbw, &bbh);
    }

    int Font::get_ranges(int ranges_count, int* ranges) const {
        return al_get_font_ranges(m_font.get(), ranges_count, ranges);
    }

    int Font::get_ranges(ranges ranges) const {
        auto conv = std::unique_ptr<int[]>(new int[ranges.size() * 2]);
        for(size_t p = 0; p < ranges.size(); ++p) {
            conv[p * 2] = ranges[p].first;
            conv[p * 2 + 1] = ranges[p].second;
        }

        return al_get_font_ranges(m_font.get(), ranges.size(), conv.get());
    }

    void Font::set_fallback(const Font& other) {
        m_fallback = other.m_font;
        al_set_fallback_font(m_font.get(), m_fallback.get());
    }

    void Font::reset_fallback() {
        al_set_fallback_font(m_font.get(), nullptr);
        m_fallback.reset();
    }

    int Font::get_glyph_width(int codepoint) const {
        return al_get_glyph_width(m_font.get(), codepoint);
    }

    bool Font::get_glyph_dimensions(int codepoint, int& bbx, int& bby, int& bbw, int& bbh) const {
        return al_get_glyph_dimensions(m_font.get(), codepoint, &bbx, &bby, &bbw, &bbh);        
    }

    int Font::get_glyph_advance(int codepoint1, int codepoint2) const {
        return al_get_glyph_advance(m_font.get(), codepoint1, codepoint2);
    }

    Font::Font(ALLEGRO_FONT* font)
        : m_font(font, al_destroy_font), m_fallback(nullptr)
    {
        if (!m_font)
            throw Exception::NullException("Font was null");
    }
    
} // namespace Graphics
} // namespace LSWE