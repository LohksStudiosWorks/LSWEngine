#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_memfile.h>

#include <memory>
#include <vector>
#include <functional>

#include <LSWE/utility/file.hpp>

namespace LSWE {
namespace Utility {

    class FileSocket : public File {
    public:
        enum class connection_type {TCP_CLIENT, TCP_LISTEN, UDP_CLIENT, UDP_BIND};

        static FileSocket create(const char* uri, uint16_t port, connection_type type = connection_type::TCP_CLIENT);

        FileSocket accept();

        using File::close;
        using File::read;
        using File::write;
        using File::eof;
        using File::get_error;
        using File::get_error_message;
        using File::clear_error;
        using File::getc;
        using File::putc;
		using File::printformat;
		using File::vprintformat;
        using File::read16le;
        using File::read16be;
        using File::read32le;
        using File::read32be;
        using File::write16le;
        using File::write16be;
        using File::write32le;
        using File::write32be;
		using File::operator<<;
		using File::operator>>;
        using File::gets;
        using File::puts;
        using File::get_ustr;
        using File::operator ALLEGRO_FILE*;
    private:
        FileSocket(ALLEGRO_FILE* file);
    };

} // namespace LSWE
} // namespace Utility