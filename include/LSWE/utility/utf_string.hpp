#pragma once

#include <allegro5/allegro5.h>

#include <string>
#include <memory>
#include <compare>

namespace LSWE {
namespace Utility {

	class UTFString {
	public:
		UTFString();
		UTFString(const char* str);
		UTFString(const std::string& str);
		UTFString(const uint16_t* const str);
		UTFString(const UTFString& str);
		UTFString(UTFString&& str) noexcept;

		void to_buffer(char* buf, int len);

		UTFString substr_bytes(int start_pos, int end_pos);
		UTFString substr(int start_pos, int end_pos);

		size_t size_bytes() const;

        size_t size() const;
		size_t length() const;

		int offset(const int index) const;

		bool next(int& pos) const;
		bool prev(int& pos) const;

		int32_t get(const int pos) const;
		int32_t operator[](const int pos) const;

		int32_t get_next(int& pos) const;
		int32_t prev_get(int& pos) const;

		bool insert_bytes(int pos, const UTFString& str);
		bool insert_bytes(int pos, const std::string& str);
		bool insert_bytes(int pos, const char* str);

		bool insert(int pos, const UTFString& str);
		bool insert(int pos, const std::string& str);
		bool insert(int pos, const char* str);

		// insert at the end
		bool append(const UTFString&);
		UTFString& operator+=(const UTFString& str);
		UTFString operator+(const UTFString& str);

		bool append(const std::string& str);
		UTFString& operator+=(const std::string& str);
		UTFString operator+(const std::string& str);

		bool append(const char* str);
		UTFString& operator+=(const char* str);
		UTFString operator+(const char* str);

		bool append(const int32_t ch);

		bool remove_bytes(int pos_byte);
		bool remove(int pos_symbol);

		bool remove_bytes(int start_pos, int end_pos);
		bool remove(int start_pos, int end_pos);

		bool truncate_bytes(int size_bytes);

		bool truncate(int size);

		bool ltrim_white_space();
		bool rtrim_white_space();
		bool trim_white_space();

		bool assign(const UTFString& str);
		bool assign(const std::string& str);
		bool assign(const char* str);

		UTFString& operator=(const UTFString& str);
		UTFString& operator=(const std::string& str);
		UTFString& operator=(const char* str);

		size_t set_bytes(int pos, const int32_t val);
		size_t set(int pos, const int32_t val);

		bool replace_bytes(int pos_start, int pos_end, const UTFString& str);
		bool replace(int pos_start, int pos_end, const UTFString& str);
				
		int find(const int32_t ch, int offset = 0, bool offset_bytes = true, bool return_bytes = true) const;
		int rfind(const int32_t ch, int offset = 0, bool offset_bytes = true, bool return_bytes = true) const;

		int find_set(const UTFString& accept_any_of, int offset = 0, bool offset_bytes = true, bool return_bytes = true) const;
		int find_cset(const UTFString& reject_any_of, int offset = 0, bool offset_bytes = true, bool return_bytes = true) const;

		int find_str(const UTFString& needle, int offset = 0, bool offset_bytes = true, bool return_bytes = true) const;        
		int find_str(const std::string& needle, int offset = 0, bool offset_bytes = true, bool return_bytes = true) const;
		int find_str(const char* needle, int offset = 0, bool offset_bytes = true, bool return_bytes = true) const;
		int rfind_str(const UTFString& needle, int offset = 0, bool offset_bytes = true, bool return_bytes = true) const;        
		int rfind_str(const std::string& needle, int offset = 0, bool offset_bytes = true, bool return_bytes = true) const;
		int rfind_str(const char* needle, int offset = 0, bool offset_bytes = true, bool return_bytes = true) const;

		bool find_replace_all(const UTFString& search, const UTFString& replace_with, int offset = 0, bool offset_bytes = true);        
		bool find_replace_all(const std::string& search, const std::string& replace_with, int offset = 0, bool offset_bytes = true);
		bool find_replace_all(const char* search, const char* replace_with, int offset = 0, bool offset_bytes = true);

		bool operator==(const UTFString& str) const;
		bool operator!=(const UTFString& str) const;
		std::strong_ordering operator<=>(const UTFString& str) const;

		bool has_prefix(const UTFString& str) const;
		bool has_suffix(const UTFString& str) const;
		bool has_prefix(const std::string& str) const;
		bool has_suffix(const std::string& str) const;
		bool has_prefix(const char* str) const;
		bool has_suffix(const char* str) const;

		std::basic_string<uint16_t> encode_utf16() const;
		std::string encode_utf8() const;

		static size_t utf8_width(const int c);
		static size_t utf8_encode(char s[], const int c);
		static size_t utf16_width(const int c);
		static size_t utf16_encode(uint16_t s[], const int c);

		std::string c_str() const;
		operator std::string() const;

		const ALLEGRO_USTR* u_str() const;
		operator const ALLEGRO_USTR*() const;
    private:
		UTFString(std::shared_ptr<ALLEGRO_USTR>&& assign);

		std::shared_ptr<ALLEGRO_USTR> m_string;
	};

} // namespace Utility
} // namespace LSWE