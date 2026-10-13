#include <LSWE/graphics/gif.hpp>

#include <allegro5/allegro_primitives.h>

#include <memory>
#include <vector>
#include <cmath>

#include <LSWE/utility/startup.hpp>
#include <LSWE/utility/file.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Graphics {

    namespace detail {
        struct RGB {
            uint8_t r{}, g{}, b{};
        };

        struct PALETTE {
            int colors_count{};
            RGB colors[256]{};
        };

        struct BITMAP {
            int w{}, h{};
            std::unique_ptr<uint8_t[]> data;
        };

        struct FRAME {
            std::unique_ptr<BITMAP> bitmap_8_bit;
            PALETTE palette{};
            int xoff{}, yoff{};
            int duration{};               // in 1/100th seconds
            int disposal_method{};        // 0 = don't care, 1 = keep, 2 = background, 3 = previous
            int transparent_index{};

            std::shared_ptr<ALLEGRO_BITMAP> rendered;
        };

        void read_palette(ALLEGRO_FILE* const& file, PALETTE& palette) {
            for (int i = 0; i < palette.colors_count; i++)
            {
                palette.colors[i].r = al_fgetc(file);
                palette.colors[i].g = al_fgetc(file);
                palette.colors[i].b = al_fgetc(file);
            }
        }

        std::unique_ptr<BITMAP> create_bitmap(int w, int h) {
            auto bitmap = std::make_unique<BITMAP>();
            bitmap->w = w;
            bitmap->h = h;
            bitmap->data = std::unique_ptr<uint8_t[]>(new uint8_t[static_cast<size_t>(w) * static_cast<size_t>(h)]);
            return bitmap;
        }

        int LZW_decode(ALLEGRO_FILE* const& file, BITMAP* bmp) {
            struct __tmp {
                int prefix;
                int c;
                int len;
            };

            int orig_bit_size{};
            char buf[256]{};
            int bit_size{};
            int bit_pos{};
            int clear_marker{};
            int end_marker{};
            
            auto codes = std::make_unique<__tmp[]>(4096); /* Maximum bit size is 12. */
            int n{};
            int i{}, prev{}, code{}, c{};
            int out_pos = 0;

            const auto read_code = [&file, &buf, &bit_pos, &bit_size]() -> int {
                int code = 0;
                int pos = 1;

                for (int i = 0; i < bit_size; ++i) {
                    int byte_pos = (bit_pos >> 3) & 255;

                    if (byte_pos == 0) {
                        int data_len = al_fgetc(file);

                        if (data_len == 0) {
                            return -1;
                        }
                        al_fread(file, buf + 256 - data_len, data_len);
                        byte_pos = 256 - data_len;
                        bit_pos = byte_pos << 3;
                    }
                    if (buf[byte_pos] & (1 << (bit_pos & 7)))
                        code += pos;
                    pos += pos;
                    ++bit_pos;
                }
                return code;
            };

            orig_bit_size = al_fgetc(file);
            n = 2 + (1 << orig_bit_size);

            for (i = 0; i < n; i++) {
                codes[i].c = i;
                codes[i].len = 0;
            }

            clear_marker = n - 2;
            end_marker = n - 1;

            bit_size = orig_bit_size + 1;

            bit_pos = 0;

            /* Expect to read clear code as first code here. */
            prev = read_code();
            if (prev == -1)
                return -1;
            do
            {
                code = read_code();
                if (code == -1) {
                    return -1;
                }
                if (code == clear_marker) {
                    bit_size = orig_bit_size;
                    n = 1 << bit_size;
                    n += 2;
                    bit_size++;
                    prev = code;
                    continue;
                }

                if (code == end_marker)
                    break;

                /* Known code: ok. Else: must be doubled char. */
                if (code < n)
                    c = code;
                else
                    c = prev;

                /* Output the code. */
                out_pos += codes[c].len;

                i = 0;
                do {
                    bmp->data[out_pos - i] = codes[c].c;
                    if (codes[c].len)
                        c = codes[c].prefix;
                    else
                        break;
                    i++;
                } while (1);

                out_pos++;

                /* Unknown code -> must be double char. */
                if (code >= n) {
                    bmp->data[out_pos] = codes[c].c;
                    out_pos++;
                }

                /* Except after clear marker, build new code. */
                if (prev != clear_marker) {
                    codes[n].prefix = prev;
                    codes[n].len = codes[prev].len + 1;
                    codes[n].c = codes[c].c;
                    n++;
                }

                /* Out of bits? Increase. */
                if (n == (1 << bit_size)) {
                    if (bit_size < 12)
                        bit_size++;
                }

                prev = code;
            } while (1);
            return 0;
        }
        
        void deinterlace(BITMAP* const& bmp)
        {
            const auto blit = [](BITMAP* const& from, BITMAP* const& to, int xf, int yf, int xt, int yt, int w, int h) {
                if (w <= 0 || h <= 0)
                    return;

                /* source clipping */
                if (xf < 0) {
                    w += xf;
                    xt -= xf;
                    xf = 0;
                }
                if (yf < 0) {
                    h += yf;
                    yt -= yf;
                    yf = 0;
                }
                int wf = from->w;
                int hf = from->h;
                if (xf + w > wf) {
                    w = wf - xf;
                }
                if (yf + h > hf) {
                    h = hf - yf;
                }

                /* destination clipping */
                if (xt < 0) {
                    w += xt;
                    xf -= xt;
                    xt = 0;
                }
                if (yt < 0) {
                    h += yt;
                    yf -= yt;
                    yt = 0;
                }
                int wt = to->w;
                int ht = to->h;
                if (xt + w > wt) {
                    w = wt - xt;
                }
                if (yt + h > ht) {
                    h = ht - yt;
                }

                if (w <= 0 || h <= 0)
                    return;

                /* copy row by row */
                uint8_t* pf = from->data.get() + yf * from->w;
                uint8_t* pt = to->data.get() + yt * to->w;
                for (int i = 0; i < h; i++) {
                    memmove(pt + xt, pf + xf, w);
                    pf += from->w;
                    pt += to->w;
                }
            };

            auto n = create_bitmap(bmp->w, bmp->h);
            int i = 0;

            for (int y = 0; y < n->h; y += 8) {
                blit(bmp, n.get(), 0, i++, 0, y, n->w, 1);
            }
            for (int y = 4; y < n->h; y += 8) {
                blit(bmp, n.get(), 0, i++, 0, y, n->w, 1);
            }
            for (int y = 2; y < n->h; y += 4) {
                blit(bmp, n.get(), 0, i++, 0, y, n->w, 1);
            }
            for (int y = 1; y < n->h; y += 2) {
                blit(bmp, n.get(), 0, i++, 0, y, n->w, 1);
            }

            blit(n.get(), bmp, 0, 0, 0, 0, n->w, n->h);
        }
    }

    struct GIF::ANIMATION {
        int width{}, height{};
        int background_index{};
        int loop{}; // -1 = no, 0 = forever, 1..65535 = that many times
        detail::PALETTE palette{};
        std::vector<detail::FRAME> frames;

        int duration{};
        double start_time{};
        std::shared_ptr<ALLEGRO_BITMAP> store;
    };

    GIF GIF::load(const std::string& path, int flags, int format) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitPrimitives>::instance().setup();

        auto fp = Utility::File::open(path.c_str(), "rb");
        auto anim = GIF::load_from_file(fp);

        return GIF(std::move(anim));
    }

    void GIF::destroy() {
        m_anim.reset();
    }

    void GIF::convert() {
        for(auto& i : m_anim->frames) {
            al_convert_bitmap(i.rendered.get());
        }
    }

    Bitmap GIF::get_frame(size_t idx) const {
        return Bitmap(m_anim->frames.at(idx).rendered.get(), true, m_anim->frames.at(idx).rendered);
    }

    size_t GIF::get_frames() const {
        return m_anim->frames.size();
    }

    int GIF::get_height() const {
        return al_get_bitmap_height(get_bitmap());
    }

    int GIF::get_width() const {
        return al_get_bitmap_width(get_bitmap());
    }    

    ALLEGRO_BITMAP* GIF::get_bitmap() const  {
        size_t n = m_anim->frames.size();

        double seconds = al_get_time() - m_anim->start_time;
        seconds = fmod(seconds, m_anim->duration / 100.0);
        double d = 0.0;
        for (int i = 0; i < n; i++) {
            d += m_anim->frames[i].duration / 100.0;
            if (seconds < d) {
                return m_anim->frames[i].rendered.get();
            }
        }
        return m_anim->frames[0].rendered.get();
    }

    GIF GIF::load(ALLEGRO_FILE* file) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitPrimitives>::instance().setup();

        auto anim = GIF::load_from_file(file);

        return GIF(std::move(anim));
    }

    GIF::GIF(std::shared_ptr<ANIMATION>&& anim)
        : m_anim(std::move(anim))
    {
        if (!m_anim)
            throw Exception::NullException("GIF was null");
        
        m_anim->start_time = al_get_time();
    }

    std::shared_ptr<GIF::ANIMATION> GIF::load_from_file(ALLEGRO_FILE* file) {
        if (!file) return {};

        // Raw load
        const auto raw_load = [file]() -> std::shared_ptr<ANIMATION> {
            auto anim = std::make_shared<ANIMATION>();

            al_fseek(file, 0, ALLEGRO_SEEK_SET);

            int version;
            std::unique_ptr<detail::BITMAP> bmp;
            int i, j;

            detail::FRAME frame;

            if (al_fgetc(file) != 'G') { return {}; }
            if (al_fgetc(file) != 'I') { return {}; }
            if (al_fgetc(file) != 'F') { return {}; }
            if (al_fgetc(file) != '8') { return {}; }

            version = al_fgetc(file);
            if (version != '7' && version != '9') { return {}; }
            if (al_fgetc(file) != 'a') { return {}; }

            anim->width = al_fread16le(file);
            anim->height = al_fread16le(file);
            i = al_fgetc(file);
            /* Global color table? */
            if (i & 128)
                anim->palette.colors_count = 1 << ((i & 7) + 1);
            else
                anim->palette.colors_count = 0;
            /* Background color is only valid with a global palette. */
            anim->background_index = al_fgetc(file);

            /* Skip aspect ratio. */
            al_fseek(file, 1, ALLEGRO_SEEK_CUR);

            if (anim->palette.colors_count) {
                detail::read_palette(file, anim->palette);
            }

            memset(&frame, 0, sizeof(frame)); /* For first frame. */
            frame.transparent_index = -1;

            do {
                i = al_fgetc(file);

                switch (i) {
                case 0x2c:         /* Image Descriptor */
                {
                    int w, h;
                    int interlaced = 0;

                    frame.xoff = al_fread16le(file);
                    frame.yoff = al_fread16le(file);
                    w = al_fread16le(file);
                    h = al_fread16le(file);
                    bmp = detail::create_bitmap(w, h);
                    i = al_fgetc(file);

                    /* Local palette. */
                    if (i & 128) {
                        frame.palette.colors_count = 1 << ((i & 7) + 1);
                        detail::read_palette(file, frame.palette);
                    }
                    else {
                        frame.palette.colors_count = 0;
                    }

                    if (i & 64)
                        interlaced = 1;

                    if (detail::LZW_decode(file, bmp.get())) {
                        return {};
                    }

                    if (interlaced)
                        detail::deinterlace(bmp.get());

                    frame.bitmap_8_bit = std::move(bmp);
                    anim->frames.push_back(std::move(frame));

                    frame = detail::FRAME{};
                    frame.transparent_index = -1;
                }
                    break;
                case 0x21: /* Extension Introducer. */
                    j = al_fgetc(file); /* Extension Type. */
                    i = al_fgetc(file); /* Size. */

                    /* Graphic Control Extension. */
                    if (j == 0xf9) { 
                        /* size must be 4 */
                        if (i != 4) { return {}; }

                        i = al_fgetc(file);
                        frame.disposal_method = (i >> 2) & 7;
                        frame.duration = al_fread16le(file);
                        if (i & 1)  /* Transparency? */
                        {
                            frame.transparent_index = al_fgetc(file);
                        }
                        else
                        {
                            al_fseek(file, 1, ALLEGRO_SEEK_CUR);
                            frame.transparent_index = -1;
                        }
                        i = al_fgetc(file); /* Size. */
                    }
                    /* Application Extension. */
                    else if (j == 0xff) {
                        if (i == 11)
                        {
                            char name[12];
                            al_fread(file, name, 11);
                            i = al_fgetc(file); /* Size. */
                            name[11] = '\0';
                            if (!strcmp(name, "NETSCAPE2.0"))
                            {
                                if (i == 3)
                                {
                                    j = al_fgetc(file);
                                    anim->loop = al_fread16le(file);
                                    if (j != 1)
                                        anim->loop = 0;
                                    i = al_fgetc(file); /* Size. */
                                }
                            }
                        }
                    }

                    /* Possibly more blocks until terminator block (0). */
                    while (i) {
                        al_fseek(file, i, ALLEGRO_SEEK_CUR);
                        i = al_fgetc(file);
                    }
                    break;
                case 0x3b:
                    /* GIF Trailer. */
                    return anim;
                }
            } while (!al_feof(file)); // was true
            return {};
        };
        
        const auto render_frame = [](std::shared_ptr<GIF::ANIMATION>& anim, size_t frame, int xpos, int ypos) {
            int x, y, w, h;
            const detail::FRAME& f = anim->frames[frame];
            
            if (frame == 0) {
                al_draw_filled_rectangle(xpos, ypos, xpos + anim->width,
                    ypos + anim->height, al_map_rgba_f(0, 0, 0, 0));
            }
            else {
                const detail::FRAME& p = anim->frames[frame - 1];
                if (p.disposal_method == 2) {
                    al_draw_filled_rectangle(xpos + p.xoff, ypos + p.yoff,
                        xpos + p.xoff + p.bitmap_8_bit->w,
                        ypos + p.yoff + p.bitmap_8_bit->h,
                        al_map_rgba_f(0, 0, 0, 0));
                }
                else if (p.disposal_method == 3 && anim->store) {
                    al_draw_bitmap_region(anim->store.get(), xpos + p.xoff, ypos + p.yoff,
                        p.bitmap_8_bit->w,
                        p.bitmap_8_bit->h,
                        xpos + p.xoff, ypos + p.yoff, 0);
                    anim->store.reset();
                }
            }

            w = f.bitmap_8_bit->w;
            h = f.bitmap_8_bit->h;

            if (f.disposal_method == 3) {
                anim->store = std::shared_ptr<ALLEGRO_BITMAP>(al_clone_bitmap(al_get_target_bitmap()), al_destroy_bitmap);
            }

            const detail::PALETTE& pal = f.palette.colors_count == 0
                ? anim->palette
                : f.palette;

            for (y = 0; y < h; y++) {
                for (x = 0; x < w; x++) {
                    int c = f.bitmap_8_bit->data[x + y * f.bitmap_8_bit->w];
                    if (c != f.transparent_index) {
                        al_draw_pixel(xpos + f.xoff + x, ypos + f.yoff + y,
                            al_map_rgb(pal.colors[c].r, pal.colors[c].g,
                                pal.colors[c].b));
                    }
                }
            }
        };

        auto anim = raw_load();
        if (!anim) return {};

        anim->duration = 0;
        ALLEGRO_STATE s;
        al_store_state(&s, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);

        for (size_t i = 0; i < anim->frames.size(); i++) {
            detail::FRAME& f = anim->frames[i];

            if (i == 0)
                f.rendered = std::shared_ptr<ALLEGRO_BITMAP>(al_create_bitmap(anim->width, anim->height), al_destroy_bitmap);
            else
                f.rendered = std::shared_ptr<ALLEGRO_BITMAP>(al_clone_bitmap(anim->frames[i - 1].rendered.get()), al_destroy_bitmap);

            al_set_target_bitmap(f.rendered.get());
            render_frame(anim, i, 0, 0);
            anim->duration += f.duration;
        }

        al_restore_state(&s);

        return anim;
    }

} // namespace Graphics
} // namespace LSWE