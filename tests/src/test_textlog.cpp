#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>

using namespace LSWE;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    auto textlog = Utility::SingletonOf<Utility::TextLog>::instance();

    textlog.printf("This should be printed in the terminal only. No window.");

    textlog.open("Testing logging system");

    textlog.printf("Hello, this app is running for %lf second(s) so far...", al_get_time());
    al_rest(1.0);
    textlog.printf("After a second, this is now %lf second(s) so far...", al_get_time());
    al_rest(1.0);
    textlog.printf("The platform is %s.", platform.c_str());
    al_rest(1.0);
    textlog.printf("Testing closing it and reopening...");
    al_rest(1.0);
    textlog.close();
    al_rest(0.5);
    textlog.open("Testing logging system");
    textlog.printf("Did it reset? Yes.");
    al_rest(5.0);

    std::cout << "PASSED!" << std::endl;
    return 0;
}