#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_memfile.h>

#include <memory>
#include <vector>
#include <functional>

namespace LSWE {
namespace Utility {

    class File {
    public:
        static File open(const char* path, const char* mode = "rb");
        static File open_interface(const ALLEGRO_FILE_INTERFACE *drv, const char *path, const char *mode);
        static File open_slice(File& oth, size_t initial_size, const char* mode); // modes: ['R'ead,'W'rite,'E'xpandable,'S'eek,'N'seeknoclose]
        static File open_fd(int fd, const char* mode);
        static File make_temp(const char* name_template = "XXXXXXXXXX.tmp", const char* mode = "wb+");
        static File open_mem(void* data, size_t len, const char* mode = "wb+");
        static File make_memory(size_t mem_len);

        void close();

        size_t read(void* ptr, size_t size) const;
        size_t write(const void* ptr, size_t size);

        bool flush();

        int64_t tell() const;
        bool seek(int64_t offset, int whence);

        bool eof() const;

        int get_error() const;
        const char* get_error_message() const;
        void clear_error();

        int ungetc(int c);
        int getc();
        int putc(int c);

		int printformat(const char* format, ...);
		int vprintformat(const char* format, va_list args);

        int16_t read16le();
        int16_t read16be();
        int32_t read32le();
        int32_t read32be();

        size_t write16le(int16_t w);
        size_t write16be(int16_t w);
        size_t write32le(int32_t w);
        size_t write32be(int32_t w);

		File& operator<<(const char* val);
		File& operator<<(const std::string& val);
		File& operator<<(const std::vector<char>& val);
		File& operator<<(bool val);
		File& operator<<(short val);
		File& operator<<(unsigned short val);
		File& operator<<(int val);
		File& operator<<(unsigned int val);
		File& operator<<(long val);
		File& operator<<(unsigned long val);
		File& operator<<(long long val);
		File& operator<<(unsigned long long val);
		File& operator<<(float val);
		File& operator<<(double val);
		File& operator<<(long double val);
		File& operator<<(std::streambuf* sb);
		File& operator<<(std::ostream& (*pf)(std::ostream&));
		File& operator<<(std::ios& (*pf)(std::ios&));
		File& operator<<(std::ios_base& (*pf)(std::ios_base&));

		File& operator>>(std::string& val);
		File& operator>>(std::vector<char>& val);
		File& operator>>(bool& val);
		File& operator>>(short& val);
		File& operator>>(unsigned short& val);
		File& operator>>(int& val);
		File& operator>>(unsigned int& val);
		File& operator>>(long& val);
		File& operator>>(unsigned long& val);
		File& operator>>(long long& val);
		File& operator>>(unsigned long long& val);
		File& operator>>(float& val);
		File& operator>>(double& val);
		File& operator>>(long double& val);

        char* gets(char* const buf, size_t max);
        int puts(char* const p);

        std::shared_ptr<ALLEGRO_USTR> get_ustr();

        int64_t size() const;

        operator ALLEGRO_FILE*() const;
    protected:
        File(ALLEGRO_FILE* file);
        File(ALLEGRO_FILE* file, std::function<void(ALLEGRO_FILE*)> destroyer);

        std::shared_ptr<ALLEGRO_FILE> m_file;
    };

} // namespace LSWE
} // namespace Utility