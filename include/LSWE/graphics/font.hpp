#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

#include <memory>
#include <span>
#include <optional>

#include <LSWE/utility/file.hpp>

namespace LSWE {
namespace Graphics {

    class Font {
    public:
        using ranges = std::span<std::pair<int, int>>;

        static Font load(const char* filename, int size, int flags = 0);
        static Font load_ttf(const char* filename, int size, int flags = 0);
        static Font load_ttf(Utility::File file, int size, int flags = 0);
        static Font load_ttf_stretch(const char* filename, int w, int h, int flags = 0);
        static Font load_ttf_stretch(Utility::File file, int w, int h, int flags = 0);
        static Font load_from_bitmap(const char* filename, int flags = 0);
        static Font create_builtin();
        static Font grab_from_bitmap(ALLEGRO_BITMAP* to_copy, int ranges_n, const int ranges[]);
        static Font grab_from_bitmap(ALLEGRO_BITMAP* to_copy, ranges ranges);

        void destroy();

        int get_line_height() const;
        int get_ascent() const;
        int get_descent() const;
        int get_width(const char* str) const;
        int get_width(const ALLEGRO_USTR* str) const;

        void draw(ALLEGRO_COLOR color, float x, float y, int flags, const char* fmt, ...);
        void draw(ALLEGRO_COLOR color, float x, float y, int flags, const ALLEGRO_USTR* str);
        void draw_justified(ALLEGRO_COLOR color, float x1, float x2, float y, float diff, int flags, const char* fmt, ...);
        void draw_justified(ALLEGRO_COLOR color, float x1, float x2, float y, float diff, int flags, const ALLEGRO_USTR* str);
        void draw_multiline(ALLEGRO_COLOR color, float x, float y, float max_width, float line_height, int flags, const char* fmt, ...);
        void draw_multiline(ALLEGRO_COLOR color, float x, float y, float max_width, float line_height, int flags, const ALLEGRO_USTR* str);
        void draw_glyph(ALLEGRO_COLOR color, float x, float y, int codepoint);

        void do_multiline(float max_width, bool(*callback)(int line_num, const char* line, int size, void* extra), void* extra, const char* fmt, ...);
        void do_multiline(float max_width, bool(*callback)(int line_num, const ALLEGRO_USTR *line, void *extra), void* extra, const ALLEGRO_USTR* str);

        void get_dimensions(const char* str, int& bbx, int& bby, int& bbw, int& bbh) const;
        void get_dimensions(const ALLEGRO_USTR* str, int& bbx, int& bby, int& bbw, int& bbh) const;
        int get_ranges(int ranges_count, int* ranges) const;
        int get_ranges(ranges ranges) const;
        void set_fallback(const Font& other);
        void reset_fallback();

        int get_glyph_width(int codepoint) const;
        bool get_glyph_dimensions(int codepoint, int& bbx, int& bby, int& bbw, int& bbh) const;
        int get_glyph_advance(int codepoint1, int codepoint2) const;
    private:
        Font(ALLEGRO_FONT* font);
        Font(ALLEGRO_FONT* font, Utility::File file);

        std::shared_ptr<ALLEGRO_FONT> m_font, m_fallback;
        std::optional<Utility::File> m_file_ref;
    };
    
} // namespace Graphics
} // namespace LSWE