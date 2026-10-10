#include <LSWE/graphics/display.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/utility/make_temporary_target.hpp>
//#include <LSWE/exception/bitmap_exception.hpp>

namespace LSWE {
namespace Graphics {

    Display Display::create(int w, int h) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();

        return Display(al_create_display(w, h));
    }

    void Display::convert_memory_bitmaps() {
        al_convert_memory_bitmaps();
    }

    void Display::hold_bitmap_drawing(bool hold) {
        al_hold_bitmap_drawing(hold);
    }

    bool Display::get_is_bitmap_drawing_held() {
        return al_is_bitmap_drawing_held();
    }

    void Display::get_new_window_pos(int& x, int& y) {
        al_get_new_window_position(&x, &y);
    }

    void Display::set_new_window_pos(int x, int y) {
        al_set_new_window_position(x, y);
    }

    int  Display::get_new_refresh_rate() {
        return al_get_new_display_refresh_rate();
    }

    void Display::set_new_refresh_rate(int refresh_rate) {
        al_set_new_display_refresh_rate(refresh_rate);
    }

    int  Display::get_new_display_adapter() {
        return al_get_new_display_adapter();
    }

    void Display::set_new_display_adapter(int adapter) {
        al_set_new_display_adapter(adapter);
    }

    void Display::set_new_window_title(const char* title) {
        al_set_new_window_title(title);
    }
    
    const char* Display::get_new_window_title() {
        return al_get_new_window_title();
    }
    
    bool Display::inhibit_screensaver(bool inhibit) {
        return al_inhibit_screensaver(inhibit);
    }

    void Display::destroy() {
        m_display.reset();
    }    

    void Display::clear_depth_buffer(float z) {
        Utility::MakeTemporaryTarget temp_target(m_backbuffer);
        al_clear_depth_buffer(z);
    }

    int Display::get_width() const {
        return al_get_display_width(m_display.get());
    }

    int Display::get_height() const {
        return al_get_display_height(m_display.get());
    }
    
    void Display::get_window_position(int& x, int& y) const {
        al_get_window_position(m_display.get(), &x, &y);
    }

    bool Display::get_window_constraints(int& min_w, int& min_h, int& max_w, int& max_h) const {
        return al_get_window_constraints(m_display.get(), &min_w, &min_h, &max_w, &max_h);
    }

    int Display::get_adapter() const {
        return al_get_display_adapter(m_display.get());
    }

    int Display::get_flags() const {
        return al_get_display_flags(m_display.get());
    }

    int Display::get_format() const {
        return al_get_display_format(m_display.get());
    }

    int Display::get_orientation() const {
        return al_get_display_orientation(m_display.get());
    }

    int Display::get_refresh_rate() const {
        return al_get_display_refresh_rate(m_display.get());
    }

    int Display::get_option(int option) const {
        return al_get_display_option(m_display.get(), option);
    }

    void Display::set_window_position(int x, int y) {
        al_set_window_position(m_display.get(), x, y);
    }

    bool Display::set_window_constraints(int min_w, int min_h, int max_w, int max_h) {
        return al_set_window_constraints(m_display.get(), min_w, min_h, max_w, max_h);
    }

    bool Display::set_flag(int flag, bool onoff) {
        return al_set_display_flag(m_display.get(), flag, onoff);
    }

    void Display::set_option(int option, int value) {
        al_set_display_option(m_display.get(), option, value);
    }

    void Display::apply_window_constraints(bool onoff) {
        al_apply_window_constraints(m_display.get(), onoff);
    }

    void Display::set_window_title(const char* title) {
        al_set_window_title(m_display.get(), title);
    }

    void Display::set_icon(const Bitmap& bitmap) {
        al_set_display_icon(m_display.get(), bitmap);
    }

    void Display::set_icons(std::span<const Bitmap> bitmaps) {
        const size_t num = bitmaps.size();
        auto bmps = std::unique_ptr<ALLEGRO_BITMAP*[]>(new ALLEGRO_BITMAP*[num]);
        size_t idx = 0;

        for(const Bitmap& bitmap : bitmaps)
            bmps[idx++] = bitmap;
        
        al_set_display_icons(m_display.get(), static_cast<int>(num), bmps.get());
    }    

    bool Display::resize(int w, int h) {
        return al_resize_display(m_display.get(), w, h);
    }

    bool Display::acknowledge_resize() {
        return al_acknowledge_resize(m_display.get());
    }

    void Display::acknowledge_drawing_halt() {
        al_acknowledge_drawing_halt(m_display.get());
    }

    void Display::acknowledge_drawing_resume() {
        al_acknowledge_drawing_resume(m_display.get());
    }

    bool Display::get_has_clipboard() const {
        return al_clipboard_has_text(m_display.get());
    }

    std::string Display::get_clipboard() const {
        char* str = al_get_clipboard_text(m_display.get());
        std::string cpy(str ? str : "");
        if (str) al_free(str);
        return cpy;
    }

    void Display::set_clipboard(const std::string& text) {
        al_set_clipboard_text(m_display.get(), text.c_str());
    }

    ALLEGRO_COLOR Display::get_blend_color() {
        Utility::MakeTemporaryTarget temp_target(m_backbuffer);
        return al_get_blend_color();
    }

    void Display::get_blender(int& op, int& src, int& dst) {
        Utility::MakeTemporaryTarget temp_target(m_backbuffer);
        return al_get_blender(&op, &src, &dst);
    }

    void Display::get_blender(int& op, int& src, int& dst, int& alpha_op, int& alpha_src, int& alpha_dst) {
        Utility::MakeTemporaryTarget temp_target(m_backbuffer);
        al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst);
    }

    void Display::set_blender(int op, int src, int dst) {
        Utility::MakeTemporaryTarget temp_target(m_backbuffer);
        al_set_blender(op, src, dst);
    }

    void Display::set_blender(int op, int src, int dst, int alpha_op, int alpha_src, int alpha_dst) {
        Utility::MakeTemporaryTarget temp_target(m_backbuffer);
        al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst);
    }

    void Display::set_blend_color(ALLEGRO_COLOR color) {
        Utility::MakeTemporaryTarget temp_target(m_backbuffer);
        al_set_blend_color(color);
    }

    int Display::get_render_state(ALLEGRO_RENDER_STATE state) {
        Utility::MakeTemporaryTarget temp_target(m_backbuffer);
        return al_get_render_state(state);
    }

    void Display::set_render_state(ALLEGRO_RENDER_STATE state, int value) {
        Utility::MakeTemporaryTarget temp_target(m_backbuffer);
        al_set_render_state(state, value);
    }

    void Display::set_as_target() {
        al_set_target_bitmap(m_backbuffer);
    }

    Bitmap Display::get_backbuffer() {
        return m_backbuffer;
    }

    void Display::flip() {
        Utility::MakeTemporaryTarget temp_target(m_backbuffer);
        al_flip_display();
    }

    void Display::flip_region(int x, int y, int w, int h) {
        Utility::MakeTemporaryTarget temp_target(m_backbuffer);
        al_update_display_region(x, y, w, h);
    }

    bool Display::wait_for_vsync() {
        Utility::MakeTemporaryTarget temp_target(m_backbuffer);
        return al_wait_for_vsync();
    }
    
    ALLEGRO_EVENT_SOURCE* Display::get_event_source() const {
        return al_get_display_event_source(m_display.get());
    }

    const Bitmap* Display::operator->() const {
        return &m_backbuffer;
    }

    Bitmap* Display::operator->() {
        return &m_backbuffer;
    }


    Display::operator ALLEGRO_EVENT_SOURCE*() const {
        return al_get_display_event_source(m_display.get());
    }

    Display::operator ALLEGRO_DISPLAY*() const {
        return m_display.get();
    }
    
    Display::Display(ALLEGRO_DISPLAY* display)
        : m_display(display, al_destroy_display), m_backbuffer(al_get_backbuffer(m_display.get()), true, {})
    {}
    
} // namespace Graphics
} // namespace LSWE