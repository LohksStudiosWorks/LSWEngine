#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

#include <memory>

namespace LSWE {
namespace Graphics {

    class Display;
    class SubBitmap;

    class Bitmap {
    public:
        static Bitmap create(int width, int height);
        static Bitmap load(const char* filename, int flags = 0);
        static Bitmap load(ALLEGRO_FILE* file, const char* ident = ".png", int flags = 0);
        static Bitmap get_target();

        Bitmap create_sub(int x, int y, int w, int h);
        Bitmap clone();

        bool save(const char* filename);
        bool save(ALLEGRO_FILE* file, const char* ident = ".png");

        static int  get_new_flags();
        static void set_new_flags(int flags);
        static void add_new_flag(int flag);
        static int  get_new_format();
        static void set_new_format(int format);
        static int  get_new_depth();
        static void set_new_depth(int format);
        static int  get_new_samples();
        static void set_new_samples(int samples);

        void draw_pixel(float x, float y, ALLEGRO_COLOR color);
        void put_pixel(int x, int y, ALLEGRO_COLOR color);
        void put_blended_pixel(int x, int y, ALLEGRO_COLOR color);
        void clear(ALLEGRO_COLOR color);
        void get_clipping_rectangle(int& x, int& y, int& w, int& h);
        void set_clipping_rectangle(int x, int y, int w, int h);
        void reset_clipping_rectangle();

        static char const* identify(const char* filename);
        static char const* identify(ALLEGRO_FILE* file);

        void destroy();

        void backup_dirty();

        void convert();
        int get_flags() const;
        int get_format() const;
        int get_height() const;
        int get_width() const;
        int get_depth() const;
        int get_samples() const;

        ALLEGRO_COLOR get_pixel(int x, int y) const;

        bool get_is_locked() const;

        bool get_is_compatible() const;
        bool get_is_sub() const;

        void get_blender(int& op, int& src, int& dst) const;
        void get_blender(int& op, int& src, int& dst, int& alpha_op, int& alpha_src, int& alpha_dst) const;
        ALLEGRO_COLOR get_blend_color() const;

        void set_blender(int op, int src, int dst);
        void set_blender(int op, int src, int dst, int alpha_op, int alpha_src, int alpha_dst);
        void set_blend_color(ALLEGRO_COLOR color);

        void reset_blend();

        void convert_mask_to_alpha(ALLEGRO_COLOR mask_color);

        void draw(float dx, float dy, int flags);
        void draw(ALLEGRO_COLOR tint, float dx, float dy, int flags);
        void draw(float cx, float cy, float dx, float dy, float angle_rad, int flags);
        void draw(ALLEGRO_COLOR tint, float cx, float cy, float dx, float dy, float angle_rad, int flags);
        void draw(float cx, float cy, float dx, float dy, float x_scale, float y_scale, float angle_rad, int flags);
        void draw(ALLEGRO_COLOR tint, float cx, float cy, float dx, float dy, float x_scale, float y_scale, float angle_rad, int flags);
        void draw(float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags);
        void draw(ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags);

        void draw_region(float sx, float sy, float sw, float sh, float dx, float dy, int flags);
        void draw_region(ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags);
        void draw_region(float sx, float sy, float sw, float sh, ALLEGRO_COLOR tint, float cx, float cy, float dx, float dy, float x_scale, float y_scale, float angle_rad, int flags);

        void set_as_target();

        operator ALLEGRO_BITMAP*() const;
    protected:
        Bitmap(ALLEGRO_BITMAP* bitmap, bool is_ref, std::shared_ptr<ALLEGRO_BITMAP> parent);
        Bitmap(std::shared_ptr<ALLEGRO_BITMAP> bitmap_ref);

        void ensure_target_is_this() const;
        void assert_not_self_target() const;

        friend class SubBitmap;
        friend class Display;

        std::shared_ptr<ALLEGRO_BITMAP> m_bitmap, m_parent;
        static thread_local ALLEGRO_BITMAP *last_target;
    };

    class SubBitmap : public Bitmap {
    public:
        static SubBitmap from(Bitmap& base, int x, int y, int w, int h);

        Bitmap get_parent() const;
        int get_sub_x() const;
        int get_sub_y() const;

        void reparent(int x, int y, int w, int h);
    private:
        SubBitmap(ALLEGRO_BITMAP* bitmap, bool is_ref, std::shared_ptr<ALLEGRO_BITMAP> parent);
    };
    
} // namespace Graphics
} // namespace LSWE