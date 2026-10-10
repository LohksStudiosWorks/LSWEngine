#include <LSWE/graphics/bitmap.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/bitmap_exception.hpp>
#include <LSWE/utility/make_temporary_target.hpp>

namespace LSWE {
namespace Graphics {

    Bitmap Bitmap::create(int width, int height) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitImage>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitPrimitives>::instance().setup();

        return Bitmap(al_create_bitmap(width, height), false, {});
    }

    Bitmap Bitmap::load(const char* filename, int flags) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitImage>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitPrimitives>::instance().setup();

        return Bitmap(al_load_bitmap_flags(filename, flags), false, {});
    }

    Bitmap Bitmap::load(ALLEGRO_FILE* file, const char* ident, int flags) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitImage>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitPrimitives>::instance().setup();

        return Bitmap(al_load_bitmap_flags_f(file, ident, flags), false, {});
    }

    Bitmap Bitmap::get_target() {
        return Bitmap(al_get_target_bitmap(), true, {});
    }

    Bitmap Bitmap::create_sub(int x, int y, int w, int h) {
        return Bitmap(al_create_sub_bitmap(m_bitmap.get(), x, y, w, h), false, m_bitmap);
    }

    Bitmap Bitmap::clone() {
        return Bitmap(al_clone_bitmap(m_bitmap.get()), false, {});
    }

    bool Bitmap::save(const char* filename) {
        return al_save_bitmap(filename, m_bitmap.get());
    }

    bool Bitmap::save(ALLEGRO_FILE* file, const char* ident) {
        return al_save_bitmap_f(file, ident, m_bitmap.get());
    }

    int Bitmap::get_new_flags() {
        return al_get_new_bitmap_flags();
    }

    void Bitmap::set_new_flags(int flags) {
        al_set_new_bitmap_flags(flags);
    }

    void Bitmap::add_new_flag(int flag) {
        al_add_new_bitmap_flag(flag);
    }

    int Bitmap::get_new_format() {
        return al_get_new_bitmap_format();
    }

    void Bitmap::set_new_format(int format) {
        al_set_new_bitmap_format(format);
    }

    int  Bitmap::get_new_depth() {
        return al_get_new_bitmap_depth();
    }

    void Bitmap::set_new_depth(int format) {
        al_set_new_bitmap_depth(format);
    }

    int  Bitmap::get_new_samples() {
        return al_get_new_bitmap_samples();
    }

    void Bitmap::set_new_samples(int samples) {
        al_set_new_bitmap_samples(samples);
    }

    void Bitmap::draw_pixel(float x, float y, ALLEGRO_COLOR color) {
        Utility::MakeTemporaryTarget temp_target(m_bitmap.get());
        al_draw_pixel(x, y, color);
    }

    void Bitmap::put_pixel(int x, int y, ALLEGRO_COLOR color) {
        Utility::MakeTemporaryTarget temp_target(m_bitmap.get());
        al_put_pixel(x, y, color);
    }

    void Bitmap::put_blended_pixel(int x, int y, ALLEGRO_COLOR color) {
        Utility::MakeTemporaryTarget temp_target(m_bitmap.get());
        al_put_blended_pixel(x, y, color);
    }

    void Bitmap::clear(ALLEGRO_COLOR color) {
        Utility::MakeTemporaryTarget temp_target(m_bitmap.get());
        al_clear_to_color(color);
    }

    void Bitmap::get_clipping_rectangle(int& x, int& y, int& w, int& h) {
        Utility::MakeTemporaryTarget temp_target(m_bitmap.get());
        al_get_clipping_rectangle(&x, &y, &w, &h);
    }

    void Bitmap::set_clipping_rectangle(int x, int y, int w, int h) {
        Utility::MakeTemporaryTarget temp_target(m_bitmap.get());
        al_set_clipping_rectangle(x, y, w, h);
    }

    void Bitmap::reset_clipping_rectangle() {
        Utility::MakeTemporaryTarget temp_target(m_bitmap.get());
        al_reset_clipping_rectangle();
    }

    char const* Bitmap::identify(const char* filename) {
        return al_identify_bitmap(filename);
    }

    char const* Bitmap::identify(ALLEGRO_FILE* file) {
        return al_identify_bitmap_f(file);
    }

    void Bitmap::destroy() {
        m_bitmap.reset();
        m_parent.reset();
    }

    void Bitmap::backup_dirty() {
        al_backup_dirty_bitmap(m_bitmap.get());
    }

    void Bitmap::convert() {
        al_convert_bitmap(m_bitmap.get());
    }

    int Bitmap::get_flags() const {
        return al_get_bitmap_flags(m_bitmap.get());
    }

    int Bitmap::get_format() const {
        return al_get_bitmap_format(m_bitmap.get());
    }

    int Bitmap::get_height() const {
        return al_get_bitmap_height(m_bitmap.get());
    }

    int Bitmap::get_width() const {
        return al_get_bitmap_width(m_bitmap.get());
    }

    int Bitmap::get_depth() const {
        return al_get_bitmap_depth(m_bitmap.get());
    }

    int Bitmap::get_samples() const {
        return al_get_bitmap_samples(m_bitmap.get());
    }

    ALLEGRO_COLOR Bitmap::get_pixel(int x, int y) const {
        return al_get_pixel(m_bitmap.get(), x, y);
    }

    bool Bitmap::get_is_locked() const {
        return al_is_bitmap_locked(m_bitmap.get());
    }

    bool Bitmap::get_is_compatible() const {
        return al_is_compatible_bitmap(m_bitmap.get());
    }

    bool Bitmap::get_is_sub() const {
        return al_is_sub_bitmap(m_bitmap.get());
    }

    void Bitmap::get_blender(int& op, int& src, int& dst) const {
        Utility::MakeTemporaryTarget temp_target(m_bitmap.get());
        return al_get_bitmap_blender(&op, &src, &dst);
    }

    void Bitmap::get_blender(int& op, int& src, int& dst, int& alpha_op, int& alpha_src, int& alpha_dst) const {
        Utility::MakeTemporaryTarget temp_target(m_bitmap.get());
        return al_get_separate_bitmap_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst);
    }

    ALLEGRO_COLOR Bitmap::get_blend_color() const {
        Utility::MakeTemporaryTarget temp_target(m_bitmap.get());
        return al_get_bitmap_blend_color();
    }

    void Bitmap::set_blender(int op, int src, int dst) {
        Utility::MakeTemporaryTarget temp_target(m_bitmap.get());
        al_set_bitmap_blender(op, src, dst);
    }

    void Bitmap::set_blender(int op, int src, int dst, int alpha_op, int alpha_src, int alpha_dst) {
        Utility::MakeTemporaryTarget temp_target(m_bitmap.get());
        al_set_separate_bitmap_blender(op, src, dst, alpha_op, alpha_src, alpha_dst);
    }

    void Bitmap::set_blend_color(ALLEGRO_COLOR color) {
        Utility::MakeTemporaryTarget temp_target(m_bitmap.get());
        al_set_bitmap_blend_color(color);
    }

    void Bitmap::reset_blend() {
        Utility::MakeTemporaryTarget temp_target(m_bitmap.get());
        al_reset_bitmap_blender();
    }

    void Bitmap::convert_mask_to_alpha(ALLEGRO_COLOR mask_color) {
        al_convert_mask_to_alpha(m_bitmap.get(), mask_color);
    }

    void Bitmap::draw(float dx, float dy, int flags) {
        assert_not_self_target();
        al_draw_bitmap(m_bitmap.get(), dx, dy, flags);
    }

    void Bitmap::draw(ALLEGRO_COLOR tint, float dx, float dy, int flags) {
        assert_not_self_target();
        al_draw_tinted_bitmap(m_bitmap.get(), tint, dx, dy, flags);
    }

    void Bitmap::draw(float cx, float cy, float dx, float dy, float angle_rad, int flags) {
        assert_not_self_target();
        al_draw_rotated_bitmap(m_bitmap.get(), cx, cy, dx, dy, angle_rad, flags);
    }

    void Bitmap::draw(ALLEGRO_COLOR tint, float cx, float cy, float dx, float dy, float angle_rad, int flags) {
        assert_not_self_target();
        al_draw_tinted_rotated_bitmap(m_bitmap.get(), tint, cx, cy, dx, dy, angle_rad, flags);
    }

    void Bitmap::draw(float cx, float cy, float dx, float dy, float x_scale, float y_scale, float angle_rad, int flags) {
        assert_not_self_target();
        al_draw_scaled_rotated_bitmap(m_bitmap.get(), cx, cy, dx, dy, x_scale, y_scale, angle_rad, flags);
    }

    void Bitmap::draw(ALLEGRO_COLOR tint, float cx, float cy, float dx, float dy, float x_scale, float y_scale, float angle_rad, int flags) {
        assert_not_self_target();
        al_draw_tinted_scaled_rotated_bitmap(m_bitmap.get(), tint, cx, cy, dx, dy, x_scale, y_scale, angle_rad, flags);
    }

    void Bitmap::draw(float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags) {
        assert_not_self_target();
        al_draw_scaled_bitmap(m_bitmap.get(), sx, sy, sw, sh, dx, dy, dw, dh, flags);
    }

    void Bitmap::draw(ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags) {
        assert_not_self_target();
        al_draw_tinted_scaled_bitmap(m_bitmap.get(), tint, sx, sy, sw, sh, dx, dy, dw, dh, flags);
    }

    void Bitmap::draw_region(float sx, float sy, float sw, float sh, float dx, float dy, int flags) {
        assert_not_self_target();
        al_draw_bitmap_region(m_bitmap.get(), sx, sy, sw, sh, dx, dy, flags);
    }

    void Bitmap::draw_region(ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags) {
        assert_not_self_target();
        al_draw_tinted_bitmap_region(m_bitmap.get(), tint, sx, sy, sw, sh, dx, dy, flags);
    }

    void Bitmap::draw_region(float sx, float sy, float sw, float sh, ALLEGRO_COLOR tint, float cx, float cy, float dx, float dy, float x_scale, float y_scale, float angle_rad, int flags) {
        assert_not_self_target();
        al_draw_tinted_scaled_rotated_bitmap_region(m_bitmap.get(), sx, sy, sw, sh, tint, cx, cy, dx, dy, x_scale, y_scale, angle_rad, flags);
    }

    void Bitmap::set_as_target() {
        al_set_target_bitmap(m_bitmap.get());
    }

    Bitmap::operator ALLEGRO_BITMAP*() const {
        return m_bitmap.get();
    }

    Bitmap::Bitmap(ALLEGRO_BITMAP* bitmap, bool is_ref, std::shared_ptr<ALLEGRO_BITMAP> parent)
        : m_bitmap(bitmap, 
            is_ref
                ? [](ALLEGRO_BITMAP* b){}
                : al_destroy_bitmap),
        m_parent(parent)
    {}

    Bitmap::Bitmap(std::shared_ptr<ALLEGRO_BITMAP> bitmap_ref)
        : m_bitmap(bitmap_ref), m_parent({})
    {}

    void Bitmap::assert_not_self_target() const {
        if (al_get_target_bitmap() == m_bitmap.get())
            throw Utility::BitmapException("Cannot target itself (you may have targeted itself in a tool method)");
    }

    SubBitmap SubBitmap::from(Bitmap& base, int x, int y, int w, int h) {
        return SubBitmap(al_create_sub_bitmap(base.m_bitmap.get(), x, y, w, h), true, base.m_bitmap);
    }

    Bitmap SubBitmap::get_parent() const {
        return Bitmap(m_parent);
    }

    int SubBitmap::get_sub_x() const {
        return al_get_bitmap_x(m_bitmap.get());
    }

    int SubBitmap::get_sub_y() const {
        return al_get_bitmap_y(m_bitmap.get());
    }

    void SubBitmap::reparent(int x, int y, int w, int h) {
        al_reparent_bitmap(m_bitmap.get(), m_parent.get(), x, y, w, h);
    }

    SubBitmap::SubBitmap(ALLEGRO_BITMAP* bitmap, bool is_ref, std::shared_ptr<ALLEGRO_BITMAP> parent)
        : Bitmap(bitmap, is_ref, parent)
    {}

} // namespace Graphics
} // namespace LSWE