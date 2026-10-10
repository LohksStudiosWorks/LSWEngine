#include <LSWE/utility/utf_string.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Utility {

	UTFString::UTFString() {
		Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
		m_string = std::shared_ptr<ALLEGRO_USTR>(al_ustr_new(""), al_ustr_free);
		if (!m_string)
			throw Exception::NullException("String was null");
	}

	UTFString::UTFString(const char* str) {
		Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
		m_string = std::shared_ptr<ALLEGRO_USTR>(al_ustr_new_from_buffer(str, strlen(str)), al_ustr_free);
		if (!m_string)
			throw Exception::NullException("String was null");
	}

	UTFString::UTFString(const std::string& str) {
		Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
		m_string = std::shared_ptr<ALLEGRO_USTR>(al_ustr_new_from_buffer(str.c_str(), str.length()), al_ustr_free);
		if (!m_string)
			throw Exception::NullException("String was null");
	}

	UTFString::UTFString(const uint16_t* const str) {
		Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
		m_string = std::shared_ptr<ALLEGRO_USTR>(al_ustr_new_from_utf16(str), al_ustr_free);
		if (!m_string)
			throw Exception::NullException("String was null");
	}

	UTFString::UTFString(const UTFString& str)
		: m_string(std::shared_ptr<ALLEGRO_USTR>(al_ustr_dup(str.m_string.get()), al_ustr_free))
	{
		if (!m_string)
			throw Exception::NullException("String was null");
	}

	UTFString::UTFString(UTFString&& str) noexcept
		: m_string(std::move(str.m_string))
	{}

	void UTFString::to_buffer(char* buf, int len) {
		al_ustr_to_buffer(m_string.get(), buf, len);
	}

	UTFString UTFString::substr_bytes(int start_pos, int end_pos) {		
		return UTFString(std::shared_ptr<ALLEGRO_USTR>(
			al_ustr_dup_substr(m_string.get(), start_pos, end_pos), 
			al_ustr_free
		));
	}

	UTFString UTFString::substr(int start_pos, int end_pos) {
		return UTFString(std::shared_ptr<ALLEGRO_USTR>(
			al_ustr_dup_substr(
				m_string.get(),
				al_ustr_offset(m_string.get(), start_pos), al_ustr_offset(m_string.get(), end_pos)
			),
			al_ustr_free
		));
	}

	size_t UTFString::size() const {
		return al_ustr_size(m_string.get());
	}

	size_t UTFString::length() const {
		return al_ustr_length(m_string.get());
	}

	int UTFString::offset(const int index) const {
		return al_ustr_offset(m_string.get(), index);
	}

	bool UTFString::next(int& i) const {
		return al_ustr_next(m_string.get(), &i);
	}

	bool UTFString::prev(int& i) const {
		return al_ustr_prev(m_string.get(), &i);
	}

	int32_t UTFString::get(const int pos) const {
		return al_ustr_get(m_string.get(), pos);
	}

	int32_t UTFString::operator[](const int pos) const {
		return get(pos);
	}

	int32_t UTFString::get_next(int& pos) const {
		return al_ustr_get_next(m_string.get(), &pos);
	}

	int32_t UTFString::prev_get(int& pos) const {
		return al_ustr_prev_get(m_string.get(), &pos);
	}

	bool UTFString::insert_bytes(int pos, const UTFString& str) {
		return al_ustr_insert(m_string.get(), pos, str.m_string.get());
	}

	bool UTFString::insert_bytes(int pos, const std::string& str) {
		return al_ustr_insert_cstr(m_string.get(), pos, str.c_str());
	}

	bool UTFString::insert_bytes(int pos, const char* str) {
		return al_ustr_insert_cstr(m_string.get(), pos, str);
	}

	bool UTFString::insert(int pos, const UTFString& str) {
		return al_ustr_insert(m_string.get(), al_ustr_offset(m_string.get(), pos), str.m_string.get());
	}

	bool UTFString::insert(int pos, const std::string& str) {
		return al_ustr_insert_cstr(m_string.get(), al_ustr_offset(m_string.get(), pos), str.c_str());
	}

	bool UTFString::insert(int pos, const char* str) {
		return al_ustr_insert_cstr(m_string.get(), al_ustr_offset(m_string.get(), pos), str);
	}

	bool UTFString::append(const UTFString& str) {
		return al_ustr_append(m_string.get(), str.m_string.get());
	}

	UTFString& UTFString::operator+=(const UTFString& str) {
		append(str);
		return *this;
	}

	UTFString UTFString::operator+(const UTFString& str) {
		UTFString cpy(*this);
		cpy.append(str);
		return cpy;
	}

	bool UTFString::append(const std::string& str) {
		return al_ustr_append_cstr(m_string.get(), str.c_str());
	}

	UTFString& UTFString::operator+=(const std::string& str) {
		append(str);
		return *this;
	}

	UTFString UTFString::operator+(const std::string& str) {
		UTFString cpy(*this);
		cpy.append(str);
		return cpy;
	}

	bool UTFString::append(const char* str) {
		return al_ustr_append_cstr(m_string.get(), str);
	}

	UTFString& UTFString::operator+=(const char* str) {
		append(str);
		return *this;
	}

	UTFString UTFString::operator+(const char* str) {
		UTFString cpy(*this);
		cpy.append(str);
		return cpy;
	}

	bool UTFString::append(const int32_t ch) {
		return al_ustr_append_chr(m_string.get(), ch);
	}

	bool UTFString::remove_bytes(int pos) {
		return al_ustr_remove_chr(m_string.get(), pos);
	}

	bool UTFString::remove(int pos) {
		return al_ustr_remove_chr(m_string.get(), al_ustr_offset(m_string.get(), pos));
	}

	bool UTFString::remove_bytes(int start_pos, int end_pos) {
		return al_ustr_remove_range(m_string.get(), start_pos, end_pos);
	}

	bool UTFString::remove(int start_pos, int end_pos) {
		return al_ustr_remove_range(m_string.get(), al_ustr_offset(m_string.get(), start_pos), al_ustr_offset(m_string.get(), end_pos));
	}

	bool UTFString::truncate_bytes(int pos) {
		return al_ustr_truncate(m_string.get(), pos);
	}

	bool UTFString::truncate(int pos) {
		return al_ustr_truncate(m_string.get(), al_ustr_offset(m_string.get(), pos));
	}

	bool UTFString::ltrim_white_space() {
		return al_ustr_ltrim_ws(m_string.get());
	}

	bool UTFString::rtrim_white_space() {
		return al_ustr_rtrim_ws(m_string.get());
	}

	bool UTFString::trim_white_space() {
		return al_ustr_trim_ws(m_string.get());
	}

	bool UTFString::assign(const UTFString& str) {
		return al_ustr_assign(m_string.get(), str.m_string.get());
	}

	bool UTFString::assign(const std::string& str) {
		return al_ustr_assign_cstr(m_string.get(), str.c_str());
	}

	bool UTFString::assign(const char* str) {
		return al_ustr_assign_cstr(m_string.get(), str);
	}

	UTFString& UTFString::operator=(const UTFString& str) {
		assign(str);
		return *this;
	}

	UTFString& UTFString::operator=(const std::string& str) {
		assign(str);
		return *this;
	}

	UTFString& UTFString::operator=(const char* str) {
		assign(str);
		return *this;
	}

	size_t UTFString::set_bytes(int pos, const int32_t val) {
		return al_ustr_set_chr(m_string.get(), pos, val);
	}

	size_t UTFString::set(int pos, const int32_t val) {
		return al_ustr_set_chr(m_string.get(), al_ustr_offset(m_string.get(), pos), val);
	}

	bool UTFString::replace_bytes(int pos_start, int pos_end, const UTFString& str) {
		return al_ustr_replace_range(m_string.get(), pos_start, pos_end, str.m_string.get());
	}

	bool UTFString::replace(int pos_start, int pos_end, const UTFString& str) {
		return al_ustr_replace_range(m_string.get(), al_ustr_offset(m_string.get(), pos_start), al_ustr_offset(m_string.get(), pos_end), str.m_string.get());
	}

	int UTFString::find(const int32_t ch, int offset, bool offset_bytes, bool return_bytes) const {
		const auto val = al_ustr_find_chr(
			m_string.get(), 
			(offset_bytes ? offset : al_ustr_offset(m_string.get(), offset)),
			ch
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}

	int UTFString::rfind(const int32_t ch, int offset, bool offset_bytes, bool return_bytes) const {
		const auto val = al_ustr_rfind_chr(
			m_string.get(), 
			(offset_bytes ? offset : al_ustr_offset(m_string.get(), offset)),
			ch
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}

	int UTFString::find_set(const UTFString& accept_any_of, int offset, bool offset_bytes, bool return_bytes) const {
		const auto val = al_ustr_find_set(
			m_string.get(), 
			(offset_bytes ? offset : al_ustr_offset(m_string.get(), offset)),
			accept_any_of.m_string.get()
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}

	int UTFString::find_cset(const UTFString& reject_any_of, int offset, bool offset_bytes, bool return_bytes) const {
		const auto val = al_ustr_find_set(
			m_string.get(), 
			(offset_bytes ? offset : al_ustr_offset(m_string.get(), offset)),
			reject_any_of.m_string.get()
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}

	int UTFString::find_str(const UTFString& needle, int offset, bool offset_bytes, bool return_bytes) const {
		const auto val = al_ustr_find_str(
			m_string.get(), 
			(offset_bytes ? offset : al_ustr_offset(m_string.get(), offset)),
			needle.m_string.get()
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}

	int UTFString::find_str(const std::string& needle, int offset, bool offset_bytes, bool return_bytes) const {
		const auto val = al_ustr_find_cstr(
			m_string.get(), 
			(offset_bytes ? offset : al_ustr_offset(m_string.get(), offset)),
			needle.c_str()
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}

	int UTFString::find_str(const char* needle, int offset, bool offset_bytes, bool return_bytes) const {
		const auto val = al_ustr_find_cstr(
			m_string.get(), 
			(offset_bytes ? offset : al_ustr_offset(m_string.get(), offset)),
			needle
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}

	int UTFString::rfind_str(const UTFString& needle, int offset, bool offset_bytes, bool return_bytes) const {
		const auto val = al_ustr_rfind_str(
			m_string.get(), 
			(offset_bytes ? offset : al_ustr_offset(m_string.get(), offset)),
			needle.m_string.get()
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}

	int UTFString::rfind_str(const std::string& needle, int offset, bool offset_bytes, bool return_bytes) const {
		const auto val = al_ustr_find_cstr(
			m_string.get(), 
			(offset_bytes ? offset : al_ustr_offset(m_string.get(), offset)),
			needle.c_str()
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}

	int UTFString::rfind_str(const char* needle, int offset, bool offset_bytes, bool return_bytes) const {
		const auto val = al_ustr_find_cstr(
			m_string.get(), 
			(offset_bytes ? offset : al_ustr_offset(m_string.get(), offset)),
			needle
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}

	bool UTFString::find_replace_all(const UTFString& search, const UTFString& replace_with, int offset, bool offset_bytes) {
		return al_ustr_find_replace(
			m_string.get(),
			(offset_bytes ? offset : al_ustr_offset(m_string.get(), offset)),
			search.m_string.get(),
			replace_with.m_string.get()
		);
	}

	bool UTFString::find_replace_all(const std::string& search, const std::string& replace_with, int offset, bool offset_bytes) {
		return al_ustr_find_replace_cstr(
			m_string.get(),
			(offset_bytes ? offset : al_ustr_offset(m_string.get(), offset)),
			search.c_str(),
			replace_with.c_str()
		);
	}

	bool UTFString::find_replace_all(const char* search, const char*  replace_with, int offset, bool offset_bytes) {
		return al_ustr_find_replace_cstr(
			m_string.get(),
			(offset_bytes ? offset : al_ustr_offset(m_string.get(), offset)),
			search,
			replace_with
		);
	}

	bool UTFString::operator==(const UTFString& str) const {
		return al_ustr_equal(m_string.get(), str.m_string.get());
	}

	bool UTFString::operator!=(const UTFString& str) const {
		return !al_ustr_equal(m_string.get(), str.m_string.get());
	}

	std::strong_ordering UTFString::operator<=>(const UTFString& str) const {
		const int res = al_ustr_compare(m_string.get(), str.m_string.get());
		return res == 0
			? std::strong_ordering::equal
			: (res < 0 ? std::strong_ordering::less : std::strong_ordering::greater);
	}

	bool UTFString::has_prefix(const UTFString& str) const {
		return al_ustr_has_prefix(m_string.get(), str.m_string.get());
	}

	bool UTFString::has_suffix(const UTFString& str) const {
		return al_ustr_has_suffix(m_string.get(), str.m_string.get());
	}

	bool UTFString::has_prefix(const std::string& str) const {
		return al_ustr_has_prefix_cstr(m_string.get(), str.c_str());
	}

	bool UTFString::has_suffix(const std::string& str) const {
		return al_ustr_has_suffix_cstr(m_string.get(), str.c_str());
	}

	bool UTFString::has_prefix(const char* str) const {
		return al_ustr_has_prefix_cstr(m_string.get(), str);
	}

	bool UTFString::has_suffix(const char* str) const {
		return al_ustr_has_suffix_cstr(m_string.get(), str);
	}

	std::basic_string<uint16_t> UTFString::encode_utf16() const {
		std::basic_string<uint16_t> utf16(al_ustr_size_utf16(m_string.get()), '\0');
		al_ustr_encode_utf16(m_string.get(), utf16.data(), utf16.size());
		return utf16;
	}

	std::string UTFString::encode_utf8() const {
		return c_str();
	}

	size_t UTFString::utf8_width(const int c) {
		return al_utf8_width(c);
	}

	size_t UTFString::utf8_encode(char s[], const int c) {
		return al_utf8_encode(s, c);
	}

	size_t UTFString::utf16_width(const int c) {
		return al_utf16_width(c);
	}

	size_t UTFString::utf16_encode(uint16_t s[], const int c) {
		return al_utf16_encode(s, c);
	}

	std::string UTFString::c_str() const {
		return al_cstr(m_string.get());
	}

	UTFString::operator std::string() const {
		return al_cstr(m_string.get());
	}

	const ALLEGRO_USTR* UTFString::u_str() const {
		return m_string.get();
	}

	UTFString::operator const ALLEGRO_USTR*() const {
		return m_string.get();
	}

	UTFString::UTFString(std::shared_ptr<ALLEGRO_USTR>&& assign)
		: m_string(std::move(assign))
	{
		if (!m_string)
			throw Exception::NullException("String was null");
	}

} // namespace Utility
} // namespace LSWE