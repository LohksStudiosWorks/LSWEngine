#include <LSWE/graphics/locked_region.hpp>

#include <LSWE/utility/make_temporary_target.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Graphics {

    LockedRegion::LockedRegion(Bitmap bmp, int format, int flags, int pos_x, int pos_y, int width, int height)
        : m_bitmap(bmp)
    {
        if (pos_x < 0) pos_x = 0;
        if (pos_y < 0) pos_y = 0;
        if (width < 0) width = m_bitmap.get_width() - pos_x;
        if (height< 0) height= m_bitmap.get_height()- pos_y;

        m_locked_region = std::shared_ptr<ALLEGRO_LOCKED_REGION>(
            al_lock_bitmap_region(bmp, pos_x, pos_y, width, height, format, flags),
            [bmp](ALLEGRO_LOCKED_REGION*){ al_unlock_bitmap(bmp); }
        );

        if (!m_locked_region)
            throw Exception::NullException("Locked region was null");
    }

	int LockedRegion::get_pixel_format_size() const {
		return al_get_pixel_size(m_locked_region->format);
	}

	int LockedRegion::get_pixel_format_bits() const {
		return al_get_pixel_format_bits(m_locked_region->format);
	}

	int LockedRegion::get_pixel_block_size() const {
		return al_get_pixel_block_size(m_locked_region->format);
	}

	int LockedRegion::get_pixel_block_width() const {
		return al_get_pixel_block_width(m_locked_region->format);
	}

	int LockedRegion::get_pixel_block_height() const {
		return al_get_pixel_block_height(m_locked_region->format);
	}

	void* LockedRegion::get_data() {
		return m_locked_region->data;
	}

	const void* LockedRegion::get_data() const {
		return m_locked_region->data;
	}

	int LockedRegion::get_format() const {
		return m_locked_region->format;
	}

	int LockedRegion::get_pitch() const {
		return m_locked_region->pitch;
	}

	int LockedRegion::get_pixel_size() const {
		return m_locked_region->pixel_size;
	}

	void LockedRegion::put_pixel(const int pos_x, const int pos_y, const ALLEGRO_COLOR color) {
        Utility::MakeTemporaryTarget temp_target(m_bitmap);
		al_put_pixel(pos_x, pos_y, color);
	}

	void LockedRegion::put_blended_pixel(const int pos_x, const int pos_y, const ALLEGRO_COLOR color) {
        Utility::MakeTemporaryTarget temp_target(m_bitmap);
		al_put_blended_pixel(pos_x, pos_y, color);
	}

} // namespace Graphics
} // namespace LSWE