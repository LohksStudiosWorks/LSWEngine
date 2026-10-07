#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>

using namespace LSWE::Utility;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;


    return 0;
}