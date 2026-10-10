#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>
#include <battery/embed.hpp>
#include <future>

using namespace LSWE;

bool tcp_test();
bool udp_test();

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;
    
    if (!tcp_test()) return 1;
    if (!udp_test()) return 1;

    std::cout << "PASSED!" << std::endl;
    return 0;
}

bool tcp_test() {
    auto sock = Utility::FileSocket::create(nullptr, 25565, Utility::FileSocket::connection_type::TCP_LISTEN);
    auto fut = std::async(std::launch::async, [&sock] { return sock.accept(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    auto cli = Utility::FileSocket::create("127.0.0.1", 25565);
    auto cli2 = fut.get();

    std::string to_send{"Hello world!"};
    std::string to_recv(16, '\0');

    cli.write(to_send.data(), to_send.size());
    cli2.read(to_recv.data(), to_recv.size());

    if (strncmp(to_send.data(), to_recv.data(), to_send.length()) != 0) {
        std::cerr << "Message mismatch: " << to_send << " != " << to_recv << std::endl;
        return false;
    }
    
    to_send = "Hi back! ;P";

    cli2.write(to_send.data(), to_send.size());
    cli.read(to_recv.data(), to_recv.size());

    if (strncmp(to_send.data(), to_recv.data(), to_send.length()) != 0) {
        std::cerr << "Message mismatch: " << to_send << " != " << to_recv << std::endl;
        return false;
    }

    std::cout << "TCP went great!" << std::endl;

    return true;
}

bool udp_test() {
    auto host = Utility::FileSocket::create(nullptr, 25565, Utility::FileSocket::connection_type::UDP_BIND);
    auto client = Utility::FileSocket::create("127.0.0.1", 25565, Utility::FileSocket::connection_type::UDP_CLIENT);

    std::string to_send{"Hello world!"};
    std::string to_recv(16, '\0');

    client.write(to_send.data(), to_send.size());
    host.read(to_recv.data(), to_recv.size());

    if (strncmp(to_send.data(), to_recv.data(), to_send.length()) != 0) {
        std::cerr << "Message mismatch: " << to_send << " != " << to_recv << std::endl;
        return false;
    }
    
    to_send = "Hi back! ;P";

    host.write(to_send.data(), to_send.size());
    client.read(to_recv.data(), to_recv.size());

    if (strncmp(to_send.data(), to_recv.data(), to_send.length()) != 0) {
        std::cerr << "Message mismatch: " << to_send << " != " << to_recv << std::endl;
        return false;
    }

    std::cout << "UDP went great!" << std::endl;

    return true;
}