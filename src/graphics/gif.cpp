#include <LSWE/graphics/gif.hpp>

#include <allegro5/allegro_primitives.h>

#include <memory>
#include <vector>
#include <cmath>
#include <cstring>

#include <LSWE/utility/startup.hpp>
#include <LSWE/utility/file.hpp>
#include <LSWE/graphics/locked_region.hpp>
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
            for (int i = 0; i < palette.colors_count; i++) {
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
            
            auto codes = std::make_unique<__tmp[]>(4096);
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
                        if (data_len == 0) return -1;
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

            prev = read_code();
            if (prev == -1) return -1;

            do {
                code = read_code();
                if (code == -1) return -1;
                if (code == clear_marker) {
                    bit_size = orig_bit_size + 1;
                    n = (1 << orig_bit_size) + 2;
                    prev = code;
                    continue;
                }
                if (code == end_marker) break;

                c = (code < n) ? code : prev;

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

                if (code >= n) {
                    bmp->data[out_pos] = codes[c].c;
                    out_pos++;
                }

                if (prev != clear_marker && n < 4096) {
                    codes[n].prefix = prev;
                    codes[n].len = codes[prev].len + 1;
                    codes[n].c = codes[c].c;
                    n++;
                }

                if (n == (1 << bit_size) && bit_size < 12) {
                    bit_size++;
                }

                prev = code;
            } while (1);
            return 0;
        }
        
        void deinterlace(BITMAP* const& bmp) {
            auto n = create_bitmap(bmp->w, bmp->h);
            int i = 0;

            const auto blit_row = [](BITMAP* from, BITMAP* to, int yf, int yt) {
                if (yf < 0 || yf >= from->h || yt < 0 || yt >= to->h) return;
                std::memcpy(to->data.get() + yt * to->w, from->data.get() + yf * from->w, from->w);
            };

            for (int y = 0; y < n->h; y += 8) blit_row(bmp, n.get(), i++, y);
            for (int y = 4; y < n->h; y += 8) blit_row(bmp, n.get(), i++, y);
            for (int y = 2; y < n->h; y += 4) blit_row(bmp, n.get(), i++, y);
            for (int y = 1; y < n->h; y += 2) blit_row(bmp, n.get(), i++, y);

            std::memcpy(bmp->data.get(), n->data.get(), static_cast<size_t>(bmp->w) * bmp->h);
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

    GIF GIF::load(ALLEGRO_FILE* file) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitPrimitives>::instance().setup();

        auto anim = GIF::load_from_file(file);

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

    ALLEGRO_BITMAP* GIF::get_bitmap() const {
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

            if (al_fgetc(file) != 'G' || al_fgetc(file) != 'I' || al_fgetc(file) != 'F' || al_fgetc(file) != '8') return {};
            int version = al_fgetc(file);
            if (version != '7' && version != '9' || al_fgetc(file) != 'a') return {};

            anim->width = al_fread16le(file);
            anim->height = al_fread16le(file);
            int i = al_fgetc(file);
            anim->palette.colors_count = (i & 128) ? (1 << ((i & 7) + 1)) : 0;
            anim->background_index = al_fgetc(file);
            al_fseek(file, 1, ALLEGRO_SEEK_CUR);

            if (anim->palette.colors_count) {
                detail::read_palette(file, anim->palette);
            }

            detail::FRAME frame{};
            frame.transparent_index = -1;

            do {
                i = al_fgetc(file);
                switch (i) {
                case 0x2c: {
                    frame.xoff = al_fread16le(file);
                    frame.yoff = al_fread16le(file);
                    int w = al_fread16le(file);
                    int h = al_fread16le(file);
                    auto bmp = detail::create_bitmap(w, h);
                    i = al_fgetc(file);

                    if (i & 128) {
                        frame.palette.colors_count = 1 << ((i & 7) + 1);
                        detail::read_palette(file, frame.palette);
                    } else {
                        frame.palette.colors_count = 0;
                    }

                    bool interlaced = (i & 64) != 0;
                    if (detail::LZW_decode(file, bmp.get())) return {};
                    if (interlaced) detail::deinterlace(bmp.get());

                    frame.bitmap_8_bit = std::move(bmp);
                    anim->frames.push_back(std::move(frame));

                    frame = detail::FRAME{};
                    frame.transparent_index = -1;
                    break;
                }
                case 0x21: {
                    int j = al_fgetc(file);
                    i = al_fgetc(file);
                    if (j == 0xf9) {
                        if (i != 4) return {};
                        i = al_fgetc(file);
                        frame.disposal_method = (i >> 2) & 7;
                        frame.duration = al_fread16le(file);
                        if (i & 1) {
                            frame.transparent_index = al_fgetc(file);
                        } else {
                            al_fseek(file, 1, ALLEGRO_SEEK_CUR);
                            frame.transparent_index = -1;
                        }
                        i = al_fgetc(file);
                    } else if (j == 0xff && i == 11) {
                        char name[12];
                        al_fread(file, name, 11);
                        i = al_fgetc(file);
                        name[11] = '\0';
                        if (!strcmp(name, "NETSCAPE2.0") && i == 3) {
                            int loop_flag = al_fgetc(file);
                            anim->loop = al_fread16le(file);
                            if (loop_flag != 1) anim->loop = 0;
                            i = al_fgetc(file);
                        }
                    }
                    while (i) {
                        al_fseek(file, i, ALLEGRO_SEEK_CUR);
                        i = al_fgetc(file);
                    }
                    break;
                }
                case 0x3b:
                    return anim;
                }
            } while (!al_feof(file));
            return anim;
        };
        
        const auto render_frame = [](std::shared_ptr<GIF::ANIMATION>& anim, size_t frame_idx, ALLEGRO_BITMAP* canvas) {
            const detail::FRAME& f = anim->frames[frame_idx];
            
            if (frame_idx > 0) {
                const detail::FRAME& p = anim->frames[frame_idx - 1];
                if (p.disposal_method == 2) {
                    ALLEGRO_STATE st;
                    al_store_state(&st, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
                    al_set_target_bitmap(canvas);
                    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
                    al_draw_filled_rectangle(p.xoff, p.yoff, p.xoff + p.bitmap_8_bit->w, p.yoff + p.bitmap_8_bit->h, al_map_rgba_f(0,0,0,0));
                    al_restore_state(&st);
                }
            }

            const detail::PALETTE& pal = f.palette.colors_count == 0 ? anim->palette : f.palette;
           
            LockedRegion region(canvas, ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE);
            
            int w = f.bitmap_8_bit->w;
            int h = f.bitmap_8_bit->h;
            const uint8_t* src_data = f.bitmap_8_bit->data.get();

            for (int y = 0; y < h; y++) {
                int target_y = f.yoff + y;
                if (target_y < 0 || target_y >= anim->height) continue;

                // Calculate row pointer in target bitmap
                uint8_t* row_ptr = (uint8_t*)region.get_data() + target_y * region.get_pitch();
                uint32_t* pixels = reinterpret_cast<uint32_t*>(row_ptr);

                for (int x = 0; x < w; x++) {
                    int target_x = f.xoff + x;
                    if (target_x < 0 || target_x >= anim->width) continue;

                    uint8_t c = src_data[x + y * w];
                    if (c != f.transparent_index) {
                        const auto& col = pal.colors[c];

                        pixels[target_x] = 0xFF000000 | 
                           (static_cast<uint32_t>(col.b) << 16) | 
                           (static_cast<uint32_t>(col.g) << 8) | 
                           static_cast<uint32_t>(col.r);
                    }
                }
            }
        };

        auto anim = raw_load();
        if (!anim) return {};

        anim->duration = 0;
        
        auto canvas = Bitmap::create(anim->width, anim->height);
        if (!canvas) return {};

        ALLEGRO_STATE s;
        al_store_state(&s, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
        al_set_target_bitmap(canvas);
        al_clear_to_color(al_map_rgba_f(0, 0, 0, 0));
        al_restore_state(&s);

        for (size_t i = 0; i < anim->frames.size(); i++) {
            render_frame(anim, i, canvas);
            
            anim->frames[i].rendered = std::shared_ptr<ALLEGRO_BITMAP>(al_clone_bitmap(canvas), al_destroy_bitmap);
            anim->duration += anim->frames[i].duration;
        }

        return anim;
    }

} // namespace Graphics
} // namespace LSWE