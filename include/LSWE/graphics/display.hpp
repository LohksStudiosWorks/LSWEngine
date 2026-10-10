#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

#include <memory>
#include <span>
#include <string>

#include <LSWE/graphics/bitmap.hpp>

namespace LSWE {
namespace Graphics {

    class Display {
    public:
        static Display create(int w, int h);

        static int  get_new_flags();
        static void set_new_flags(int flags = 0);
        static int  get_new_option(int option);
        static void set_new_option(int option, int value, int importance = ALLEGRO_SUGGEST);
        static void reset_new_options();

        static void convert_memory_bitmaps();
        static void hold_bitmap_drawing(bool hold);
        static bool get_is_bitmap_drawing_held();
        static void get_new_window_pos(int& x, int& y);
        static void set_new_window_pos(int x, int y);
        static int  get_new_refresh_rate();
        static void set_new_refresh_rate(int refresh_rate = 0);
        static int  get_new_display_adapter();
        static void set_new_display_adapter(int adapter = ALLEGRO_DEFAULT_DISPLAY_ADAPTER);
        static void set_new_window_title(const char* title);
        static const char* get_new_window_title();
        static bool inhibit_screensaver(bool inhibit);

        void destroy();

        void clear_depth_buffer(float z);

        int get_width() const;
        int get_height() const;
        void get_window_position(int& x, int& y) const;
        bool get_window_constraints(int& min_w, int& min_h, int& max_w, int& max_h) const;
        int get_adapter() const;
        int get_flags() const;
        int get_format() const;
        int get_orientation() const;
        int get_refresh_rate() const;
        int get_option(int option) const;

        void set_window_position(int x, int y);
        bool set_window_constraints(int min_w, int min_h, int max_w, int max_h);
        bool set_flag(int flag, bool onoff);
        void set_option(int option, int value);

        void apply_window_constraints(bool onoff);

        void set_window_title(const char* title);
        void set_icon(const Bitmap& bitmap);
        void set_icons(std::span<const Bitmap> bitmaps);

        bool resize(int w, int h);

        bool acknowledge_resize();
        void acknowledge_drawing_halt();
        void acknowledge_drawing_resume();

        bool get_has_clipboard() const;
        std::string get_clipboard() const;
        void set_clipboard(const std::string& text);

        ALLEGRO_COLOR get_blend_color();
        void get_blender(int& op, int& src, int& dst);
        void get_blender(int& op, int& src, int& dst, int& alpha_op, int& alpha_src, int& alpha_dst);
        void set_blender(int op, int src, int dst);
        void set_blender(int op, int src, int dst, int alpha_op, int alpha_src, int alpha_dst);
        void set_blend_color(ALLEGRO_COLOR color);

        int get_render_state(ALLEGRO_RENDER_STATE state);
        void set_render_state(ALLEGRO_RENDER_STATE state, int value);

        void set_as_target();

        Bitmap get_backbuffer();

        void flip();
        void flip_region(int x, int y, int w, int h);
        bool wait_for_vsync();

        ALLEGRO_EVENT_SOURCE* get_event_source() const;

        const Bitmap* operator->() const;
        Bitmap* operator->();
        
        operator ALLEGRO_EVENT_SOURCE*() const;
        operator ALLEGRO_DISPLAY*() const;
    private:
        Display(ALLEGRO_DISPLAY* display);

        std::shared_ptr<ALLEGRO_DISPLAY> m_display;
        Bitmap m_backbuffer;
    };

    
} // namespace Graphics
} // namespace LSWE