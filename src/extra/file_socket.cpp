#include <LSWE/extra/file_socket.hpp>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET socket_t;
    #define IS_INVALID_SOCKET(s) ((s) == INVALID_SOCKET)
    #define CLOSE_SOCKET(s) closesocket(s)
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <errno.h>
    typedef int socket_t;
    #define INVALID_SOCKET (-1)
    #define IS_INVALID_SOCKET(s) ((s) < 0)
    #define CLOSE_SOCKET(s) close(s)
#endif

#include <LSWE/exception/file_exception.hpp>

namespace LSWE {
namespace Utility {

    const ALLEGRO_FILE_INTERFACE *al_get_socket_file_interface();

    struct socket_config {
        const char* host;
        FileSocket::connection_type type;
        uint16_t port;
    };

    struct socket_context {
        FileSocket::connection_type original_type;
        socket_t sock = INVALID_SOCKET;
        int domain = AF_INET; // AF_INET || AF_INET6
        int type = SOCK_STREAM; // SOCK_STREAM || SOCK_DGRAM
        int last_error = 0;
        char err_msg[256];
        

        sockaddr_storage remote_addr{};
        socklen_t remote_addr_len{};
        bool has_remote_addr = false;

        bool is_listening = false;
        bool eof = false;
    };

    FileSocket FileSocket::create(const char* uri, uint16_t port, connection_type type) {        
        socket_config cfg {
            .host = uri,
            .type = type,
            .port = port
        };
        const ALLEGRO_FILE_INTERFACE* interface = al_get_socket_file_interface();
        const size_t cfg_len = sizeof(cfg);

        return FileSocket(al_fopen_interface(interface, (char*)&cfg, (char*)&cfg_len));
    }

    FileSocket FileSocket::accept() {
        socket_context *ctx = (socket_context*)al_get_file_userdata(m_file.get());

        if (ctx->original_type != FileSocket::connection_type::TCP_LISTEN)
            throw FileException("Tried accept on a non TCP Listen socket file!");

        socket_context *client_ctx = new socket_context();


        if (!this->File::read((char*)client_ctx, sizeof(socket_context))) {
            delete client_ctx;
            throw FileException("Failed to accept new socket client");
        }

        return FileSocket(al_create_file_handle(al_get_socket_file_interface(), client_ctx));
    }

    FileSocket::FileSocket(ALLEGRO_FILE* file)
        : File::File(file)
    {}

    namespace detail {
        
        static bool g_sockets_initialized = false;

        static bool al_init_socket_interface();
        static void al_shutdown_socket_interface();
        static addrinfo *resolve_address(const char *host, uint16_t port, FileSocket::connection_type type);
        static bool parse_uri(const char *path, FileSocket::connection_type& out_type, char *out_host, size_t host_len, uint16_t& out_port);

        static void *sock_open(const char *path, const char *mode) {
            if (!g_sockets_initialized) al_init_socket_interface();

            const socket_config* scfg = (socket_config*)path;
            const size_t* scfg_len_check = (size_t*)mode;

            if (!scfg || !scfg_len_check) return nullptr;
            if (*scfg_len_check != sizeof(socket_config)) return nullptr;

            const FileSocket::connection_type type = scfg->type;
            char host[256] = {0};
            const uint16_t port = scfg->port;
            if (scfg->host) snprintf(host, sizeof(host), "%s", scfg->host);

            socket_context *ctx = new socket_context();

            struct addrinfo *info = resolve_address(host, port, type);
            if (!info) {
                delete ctx;
                return nullptr;
            }

            ctx->original_type = scfg->type;
            ctx->domain = info->ai_family;
            ctx->type = info->ai_socktype;
            ctx->sock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
            if (IS_INVALID_SOCKET(ctx->sock)) {
                freeaddrinfo(info);
                delete ctx;
                return nullptr;
            }

            // Enable Dual-Stack (IPv4 + IPv6) for IPv6 listening/binding sockets
            if (info->ai_family == AF_INET6 && (type == FileSocket::connection_type::TCP_LISTEN || type == FileSocket::connection_type::UDP_BIND)) {
                int v6only = 0;
                setsockopt(ctx->sock, IPPROTO_IPV6, IPV6_V6ONLY, (const char *)&v6only, sizeof(v6only));
            }

            // Allow address reuse
            if (type == FileSocket::connection_type::TCP_LISTEN ||
                type == FileSocket::connection_type::UDP_BIND)
            {
                int opt = 1;
                setsockopt(ctx->sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
            }
            
            switch(type) {
            case FileSocket::connection_type::TCP_CLIENT:
                if (connect(ctx->sock, info->ai_addr, (socklen_t)info->ai_addrlen) < 0) {
                    CLOSE_SOCKET(ctx->sock);
                    freeaddrinfo(info);
                    delete ctx;
                    return nullptr;
                }
                break;
            case FileSocket::connection_type::TCP_LISTEN:
                if (bind(ctx->sock, info->ai_addr, (socklen_t)info->ai_addrlen) < 0 ||
                    listen(ctx->sock, 5) < 0) {
                    CLOSE_SOCKET(ctx->sock);
                    freeaddrinfo(info);
                    delete ctx;
                    return nullptr;
                }
                ctx->is_listening = true;
                break;
            case FileSocket::connection_type::UDP_BIND:
                if (bind(ctx->sock, info->ai_addr, (socklen_t)info->ai_addrlen) < 0) {
                    CLOSE_SOCKET(ctx->sock);
                    freeaddrinfo(info);
                    delete ctx;
                    return nullptr;
                }
                break;
            case FileSocket::connection_type::UDP_CLIENT:
                memcpy(&ctx->remote_addr, info->ai_addr, info->ai_addrlen);
                ctx->remote_addr_len = (socklen_t)info->ai_addrlen;
                ctx->has_remote_addr = true;
                break;
            }

            freeaddrinfo(info);
            return ctx;
        }

        static bool sock_close(ALLEGRO_FILE *f) {
            socket_context *ctx = (socket_context*)al_get_file_userdata(f);
            if (!ctx) return false;

            if (!IS_INVALID_SOCKET(ctx->sock)) {
                CLOSE_SOCKET(ctx->sock);
                ctx->sock = INVALID_SOCKET;
            }
            delete ctx;

            return true;
        }


        // multi purpose:
        // or ptr -> char*
        // or ptr -> socket_context*
        static size_t sock_read(ALLEGRO_FILE *f, void *ptr, size_t size) {
            socket_context *ctx = (socket_context*)al_get_file_userdata(f);
            if (!ctx || ctx->eof || size == 0) return 0;

            uint8_t *buf = (uint8_t *)ptr;
            size_t read_bytes = 0;

            int bytes_received = 0;

            switch(ctx->original_type) {
            case FileSocket::connection_type::UDP_CLIENT:
            case FileSocket::connection_type::UDP_BIND:
                {
                    struct sockaddr_storage from;
                    socklen_t fromlen = sizeof(from);
                    bytes_received = recvfrom(ctx->sock, (char *)buf, (int)size, 0, (struct sockaddr *)&from, &fromlen);
                    if (bytes_received > 0) {
                        ctx->remote_addr = from;
                        ctx->remote_addr_len = fromlen;
                        ctx->has_remote_addr = true;
                    }
                }
                break;
            case FileSocket::connection_type::TCP_CLIENT:
                bytes_received = recv(ctx->sock, (char *)buf, (int)size, 0);
                break;
            case FileSocket::connection_type::TCP_LISTEN:
                if (size != sizeof(socket_context) || !ctx->is_listening)
                    return 0;
                
                {
                    socket_context* client_ctx = (socket_context*)ptr;

                    struct sockaddr_storage client_addr;
                    socklen_t addrlen = sizeof(client_addr);
                    socket_t client_sock = accept(ctx->sock, (struct sockaddr *)&client_addr, &addrlen);

                    if (IS_INVALID_SOCKET(client_sock)) return 0;

                    client_ctx->original_type = FileSocket::connection_type::TCP_CLIENT;
                    client_ctx->sock = client_sock;
                    client_ctx->domain = client_addr.ss_family;
                    client_ctx->type = SOCK_STREAM;
                    client_ctx->remote_addr = client_addr;
                    client_ctx->remote_addr_len = addrlen;
                    client_ctx->has_remote_addr = true;

                    return 1;
                }
            }

            if (bytes_received == 0) {
                ctx->eof = true;
            } else if (bytes_received < 0) {
                ctx->last_error =
#ifdef _WIN32
                    WSAGetLastError();
#else
                    errno;
#endif
                return read_bytes;
            }

            return read_bytes + (size_t)bytes_received;
        }

        static size_t sock_write(ALLEGRO_FILE *f, const void *ptr, size_t size) {
            socket_context *ctx = (socket_context*)al_get_file_userdata(f);
            if (!ctx || size == 0) return 0;

            int bytes_sent = 0;
            if (ctx->type == SOCK_DGRAM) {
                if (!ctx->has_remote_addr) return 0;
                bytes_sent = sendto(ctx->sock, (const char *)ptr, (int)size, 0,
                                    (struct sockaddr *)&ctx->remote_addr, ctx->remote_addr_len);
            } else {
                bytes_sent = send(ctx->sock, (const char *)ptr, (int)size, 0);
            }

            if (bytes_sent < 0) {
                ctx->last_error =
#ifdef _WIN32
                    WSAGetLastError();
#else
                    errno;
#endif
                return 0;
            }

            return (size_t)bytes_sent;
        }

        static bool sock_flush(ALLEGRO_FILE *f) {
            (void)f;
            return true;
        }

        static int64_t sock_tell(ALLEGRO_FILE *f) {
            (void)f;
            return -1;
        }

        static bool sock_seek(ALLEGRO_FILE *f, int64_t offset, int whence) {
            (void)f;
            (void)offset;
            (void)whence;

            return false;
        }

        static bool sock_eof(ALLEGRO_FILE *f) {
            socket_context *ctx = (socket_context*)al_get_file_userdata(f);
            return ctx ? ctx->eof : true;
        }

        static int sock_error(ALLEGRO_FILE *f) {
            socket_context *ctx = (socket_context*)al_get_file_userdata(f);
            return ctx ? ctx->last_error : -1;
        }

        static const char *sock_errmsg(ALLEGRO_FILE *f) {
            socket_context *ctx = (socket_context*)al_get_file_userdata(f);
            if (!ctx) return "Invalid context";
            snprintf(ctx->err_msg, sizeof(ctx->err_msg), "Socket error code: %d", ctx->last_error);
            return ctx->err_msg;
        }

        static void sock_clearerr(ALLEGRO_FILE *f) {
            socket_context *ctx = (socket_context*)al_get_file_userdata(f);
            if (ctx) {
                ctx->last_error = 0;
                ctx->eof = false;
            }
        }

        static int sock_ungetc(ALLEGRO_FILE *f, int c) {
            (void)f;
            (void)c;
            return EOF;
        }

        static off_t sock_size(ALLEGRO_FILE *f) {
            (void)f;
            return 0;
        }

        static const ALLEGRO_FILE_INTERFACE socket_interface = {
            sock_open,
            sock_close,
            sock_read,
            sock_write,
            sock_flush,
            sock_tell,
            sock_seek,
            sock_eof,
            sock_error,
            sock_errmsg,
            sock_clearerr,
            sock_ungetc,
            sock_size
        };

        bool al_init_socket_interface() {
            if (g_sockets_initialized) return true;
    #ifdef _WIN32
            WSADATA wsa;
            if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
                return false;
            }
    #endif
            g_sockets_initialized = true;
            return true;
        }

        void al_shutdown_socket_interface() {
            if (!g_sockets_initialized) return;
    #ifdef _WIN32
            WSACleanup();
    #endif
            g_sockets_initialized = false;
        }

        addrinfo *resolve_address(const char *host, uint16_t port, FileSocket::connection_type type) {
            struct addrinfo hints;
            struct addrinfo *res = nullptr;
            char port_str[6];
            snprintf(port_str, sizeof(port_str), "%u", port);

            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_UNSPEC; // Supports both IPv4 (AF_INET) and IPv6 (AF_INET6)
            hints.ai_socktype = (type == FileSocket::connection_type::TCP_CLIENT || type == FileSocket::connection_type::TCP_LISTEN) 
                                ? SOCK_STREAM : SOCK_DGRAM;

            if (type == FileSocket::connection_type::TCP_LISTEN || type == FileSocket::connection_type::UDP_BIND) {
                hints.ai_flags = AI_PASSIVE; // Fills in INADDR_ANY / in6addr_any automatically
            }

            // Treat empty, "*", "0.0.0.0", or "::" as wildcard bind
            const char *node = nullptr;
            if (host && strlen(host) > 0 && strcmp(host, "*") != 0 && 
                strcmp(host, "0.0.0.0") != 0 && strcmp(host, "::") != 0) {
                node = host;
            }

            if (getaddrinfo(node, port_str, &hints, &res) != 0) {
                return nullptr;
            }
            return res; // Caller must call freeaddrinfo(res)
        }

        bool parse_uri(const char *path, FileSocket::connection_type& out_type, char *out_host, size_t host_len, uint16_t& out_port) {
            char proto[32] = {0};
            char host_port[256] = {0};

            if (sscanf(path, "%31[^:]://%255s", proto, host_port) != 2) {
                return false;
            }

            if (strcmp(proto, "tcp") == 0) out_type = FileSocket::connection_type::TCP_CLIENT;
            else if (strcmp(proto, "tcplisten") == 0) out_type = FileSocket::connection_type::TCP_LISTEN;
            else if (strcmp(proto, "udp") == 0) out_type = FileSocket::connection_type::UDP_CLIENT;
            else if (strcmp(proto, "udpbind") == 0) out_type = FileSocket::connection_type::UDP_BIND;
            else return false;

            // Check for bracketed IPv6 notation: [::1]:8080
            if (host_port[0] == '[') {
                char *close_bracket = strchr(host_port, ']');
                if (!close_bracket) return false;

                *close_bracket = '\0';
                snprintf(out_host, host_len, "%s", host_port + 1); // Extract IP inside brackets

                char *colon = strchr(close_bracket + 1, ':');
                out_port = colon ? (uint16_t)atoi(colon + 1) : 0;
            } else {
                // Standard IPv4 / Hostname parsing: 127.0.0.1:8080 or localhost:8080
                char *colon = strrchr(host_port, ':');
                if (colon) {
                    *colon = '\0';
                    snprintf(out_host, host_len, "%s", host_port);
                    out_port = (uint16_t)atoi(colon + 1);
                } else {
                    out_host[0] = '\0';
                    out_port = (uint16_t)atoi(host_port);
                }
            }
            return true;
        }

    }

    const ALLEGRO_FILE_INTERFACE *al_get_socket_file_interface() {
        return &detail::socket_interface;
    }
} // namespace LSWE
} // namespace Utility