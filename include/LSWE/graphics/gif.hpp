#pragma once

#include <allegro5/allegro5.h>

#include <LSWE/graphics/bitmap.hpp>

namespace LSWE {
namespace Graphics {

    class GIF : public DrawableBitmap {
    public:
        static GIF load(const std::string& path, int flags = ALLEGRO_VIDEO_BITMAP, int format = 0);
        static GIF load(ALLEGRO_FILE* file);

        void destroy();

        void convert();

        Bitmap get_frame(size_t idx) const;
        size_t get_frames() const;

        int get_height() const;
        int get_width() const;
    private:
        struct ANIMATION;

        GIF(std::shared_ptr<ANIMATION>&& anim);

        std::shared_ptr<ANIMATION> m_anim;

        ALLEGRO_BITMAP* get_bitmap() const override;

        static std::shared_ptr<ANIMATION> load_from_file(ALLEGRO_FILE* file);
    };
    
} // namespace Graphics
} // namespace LSWE