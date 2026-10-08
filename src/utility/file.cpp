#include <LSWE/utility/file.hpp>

#include <memory>
#include <sstream>
#include <stdarg.h>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/file_exception.hpp>

namespace LSWE {
namespace Utility {

    File File::open(const char* path, const char* mode) {
        SingletonOf<AllegroInit>::instance().setup();

        return File(al_fopen(path, mode));
    }

    File File::open_interface(const ALLEGRO_FILE_INTERFACE *drv, const char *path, const char *mode) {
        SingletonOf<AllegroInit>::instance().setup();

        return File(al_fopen_interface(drv, path, mode));
    }

    File File::open_slice(File& oth, size_t initial_size, const char* mode) {
        SingletonOf<AllegroInit>::instance().setup();

        return File(al_fopen_slice(oth.m_file.get(), initial_size, mode));
    }

    File File::open_fd(int fd, const char* mode) {
        SingletonOf<AllegroInit>::instance().setup();

        return File(al_fopen_fd(fd, mode));
    }

    File File::make_temp(const char* name_template, const char* mode) {
        SingletonOf<AllegroInit>::instance().setup();

        ALLEGRO_PATH* tmpptr = al_create_path(nullptr);

		auto* fpp = al_make_temp_file(name_template, &tmpptr);
		if (!fpp) 
            throw FileException("Could not open temp file!");

		std::string temp_path = al_path_cstr(tmpptr, ALLEGRO_NATIVE_PATH_SEP);
		al_destroy_path(tmpptr);
		al_fclose(fpp);

        return File(al_fopen(temp_path.c_str(), mode), [temp_path](ALLEGRO_FILE* fp) {
            al_fclose(fp);
            if (temp_path.empty()) return;
            std::remove(temp_path.c_str());
        });
    }

    File File::open_mem(void* data, size_t len, const char* mode) {
        SingletonOf<AllegroInit>::instance().setup();

        return File(al_open_memfile(data, len, mode));
    }

    File File::make_memory(size_t mem_len) {
        SingletonOf<AllegroInit>::instance().setup();

        auto mem = std::shared_ptr<char[]>(new char[mem_len]);

        return File(al_open_memfile(mem.get(), mem_len, "wb"), [mem](ALLEGRO_FILE* fp) {
            al_fclose(fp);
        });
    }

    void File::close() {
        m_file.reset();
    }

    size_t File::read(void* ptr, size_t size) const {
        return al_fread(m_file.get(), ptr, size);
    }

    size_t File::write(const void* ptr, size_t size) {
        return al_fwrite(m_file.get(), ptr, size);
    }

    bool File::flush() {
        return al_fflush(m_file.get());
    }

    int64_t File::tell() const {
        return al_ftell(m_file.get());
    }

    bool File::seek(int64_t offset, int whence) {
        return al_fseek(m_file.get(), offset, whence);
    }

    bool File::eof() const {
        return al_feof(m_file.get());
    }

    int File::get_error() const {
        return al_ferror(m_file.get());
    }

    const char* File::get_error_message() const {
        return al_ferrmsg(m_file.get());
    }

    void File::clear_error() {
        al_fclearerr(m_file.get());
    }

    int File::ungetc(int c) {
        return al_fungetc(m_file.get(), c);
    }

    int File::getc() {
        return al_fgetc(m_file.get());
    }

    int File::putc(int c) {
        return al_fputc(m_file.get(), c);
    }

	int File::printformat(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		auto ret = al_vfprintf(m_file.get(), format, args);
		va_end(args);
		return ret;
	}

	int File::vprintformat(const char* format, va_list args)
	{
		return al_vfprintf(m_file.get(), format, args);
	}

	int16_t File::read16le()
	{
		return al_fread16le(m_file.get());
	}

	int16_t File::read16be()
	{
		return al_fread16be(m_file.get());
	}

	size_t File::write16le(int16_t c)
	{
		return al_fwrite16le(m_file.get(), c);
	}

	size_t File::write16be(int16_t c)
	{
		return al_fwrite16le(m_file.get(), c);
	}

	int32_t File::read32le()
	{
		return al_fread32le(m_file.get());
	}

	int32_t File::read32be()
	{
		return al_fread32be(m_file.get());
	}

	size_t File::write32le(int32_t c)
	{
		return al_fwrite32le(m_file.get(), c);
	}

	size_t File::write32be(int32_t c)
	{
		return al_fwrite32le(m_file.get(), c);
	}

	File& File::operator<<(const char* val)
	{
		this->write(val, strlen(val));
		return *this;
	}

	File& File::operator<<(const std::string& val)
	{
		this->write(val.data(), val.size());
		return *this;
	}

	File& File::operator<<(const std::vector<char>& val)
	{
		this->write(val.data(), val.size());
		return *this;
	}

	File& File::operator<<(bool val)
	{
		const std::string str = (val ? "1" : "0");
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(short val)
	{
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(unsigned short val)
	{
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(int val)
	{
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(unsigned int val)
	{
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(long val)
	{
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(unsigned long val)
	{
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(long long val)
	{
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(unsigned long long val)
	{
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(float val)
	{
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(double val)
	{
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(long double val)
	{
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(std::streambuf* sb)
	{	
		std::stringstream ss;
		ss << sb;
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(std::ostream& (*pf)(std::ostream&))
	{
		std::stringstream ss;
		ss << pf;
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(std::ios& (*pf)(std::ios&))
	{
		std::stringstream ss;
		ss << pf;
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(std::ios_base& (*pf)(std::ios_base&))
	{
		std::stringstream ss;
		ss << pf;
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator>>(std::string& val)
	{
        constexpr size_t max = static_cast<size_t>(1) << 12;
		val = std::string(max, '\0');
		al_fgets(m_file.get(), val.data(), val.size());
		size_t exp_siz = strnlen(val.data(), max);
		if (exp_siz < val.size()) val.resize(exp_siz);

		return *this;
	}

	File& File::operator>>(std::vector<char>& val)
	{
		std::string tmp;
        *this >> tmp;
		val.assign(std::move_iterator(tmp.begin()), std::move_iterator(tmp.end()));
		return *this;
	}

	File& File::operator>>(bool& val)
	{
		char _tmp{};
		val = (this->read(&_tmp, sizeof(_tmp)) == 0) ? false : (_tmp == '1');
		return *this;
	}

	File& File::operator>>(short& val)
	{
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && (std::isdigit(_tmp) || _tmp == '-') && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
#ifdef _WIN32
			if (sscanf_s(_buf.data(), "%hd", &val) == 1) return *this;
			val = 0;
#else
			if (sscanf(_buf.data(), "%hd", &val) == 1) return *this;
			val = 0;
#endif
		}
		return *this;
	}

	File& File::operator>>(unsigned short& val)
	{
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
#ifdef _WIN32
			if (sscanf_s(_buf.data(), "%hu", &val) == 1) return *this;
			val = 0;
#else
			if (sscanf(_buf.data(), "%hu", &val) == 1) return *this;
			val = 0;
#endif
		}
		return *this;
	}

	File& File::operator>>(int& val)
	{
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stoi(_buf);
		}
		return *this;
	}

	File& File::operator>>(unsigned int& val)
	{
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
#ifdef _WIN32
			if (sscanf_s(_buf.data(), "%u", &val) == 1) return *this;
			val = 0;
#else
			if (sscanf(_buf.data(), "%u", &val) == 1) return *this;
			val = 0;
#endif
		}
		return *this;
	}

	File& File::operator>>(long& val)
	{
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stol(_buf);
		}
		return *this;
	}

	File& File::operator>>(unsigned long& val)
	{
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stoul(_buf);
		}
		return *this;
	}

	File& File::operator>>(long long& val)
	{
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stoll(_buf);
		}
		return *this;
	}

	File& File::operator>>(unsigned long long& val)
	{
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stoull(_buf);
		}
		return *this;
	}

	File& File::operator>>(float& val)
	{
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stof(_buf);
		}
		return *this;
	}

	File& File::operator>>(double& val)
	{
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stod(_buf);
		}
		return *this;
	}

	File& File::operator>>(long double& val)
	{
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stold(_buf);
		}
		return *this;
	}

    char* File::gets(char* const buf, size_t max) {
		return al_fgets(m_file.get(), buf, max);
    }

    int File::puts(char* const p) {
		return al_fputs(m_file.get(), p);
    }

	std::shared_ptr<ALLEGRO_USTR> File::get_ustr()
	{
		return std::shared_ptr<ALLEGRO_USTR>(al_fget_ustr(m_file.get()), [](ALLEGRO_USTR* u) { al_ustr_free(u); });
	}

    int64_t File::size() const {
        return al_fsize(m_file.get());
    }

    File::operator ALLEGRO_FILE*() const {
        return m_file.get();
    }

    File::File(ALLEGRO_FILE* file) 
        : m_file(file, al_fclose)
    {}

    File::File(ALLEGRO_FILE* file, std::function<void(ALLEGRO_FILE*)> destroyer) 
        : m_file(file, destroyer)
    {}
	
} // namespace LSWE
} // namespace Utility