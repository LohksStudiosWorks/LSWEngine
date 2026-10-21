#pragma once

#include <allegro5/allegro5.h>

#include <memory>

namespace LSWE {
namespace Graphics {

    class LockedRegion {
    public:
        LockedRegion(ALLEGRO_BITMAP* bmp, int format = ALLEGRO_PIXEL_FORMAT_ANY, int flags = ALLEGRO_LOCK_READWRITE, int pos_x = -1, int pos_y = -1, int width = -1, int height = -1);

		int get_pixel_format_size() const;
		int get_pixel_format_bits() const;
		int get_pixel_block_size() const;
		int get_pixel_block_width() const;
		int get_pixel_block_height() const;

		void* get_data();
		const void* get_data() const;
		int get_format() const;
		int get_pitch() const;
		int get_pixel_size() const;

		void put_pixel(const int pos_x, const int pos_y, const ALLEGRO_COLOR color);
		void put_blended_pixel(const int pos_x, const int pos_y, const ALLEGRO_COLOR color);
    private:
        ALLEGRO_BITMAP* m_bitmap;

        std::shared_ptr<ALLEGRO_LOCKED_REGION> m_locked_region;
    };
    
} // namespace Graphics
} // namespace LSWE