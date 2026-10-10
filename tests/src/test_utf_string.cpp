#include <LSWE/lswe.hpp>
#include <iostream>

using namespace LSWE;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    Utility::UTFString str1;
    Utility::UTFString str2("Hello world!");

    str1 = str2;
    str2 = "Changed it!";

    if (str2 == str1) {
        std::cerr << "Error: it was referenced, not copied?" << std::endl;
        return 1;
    }
    if (str1 != "Hello world!") {
        std::cerr << "Error: copy not copied right." << std::endl;
        return 1;
    }

    str1 += " banana";

    if (str1 != "Hello world! banana") {
        std::cerr << "Error: append does not work." << std::endl;
        return 1;
    }

    str1 = "abcde";
    str2 = "abdde";

    if (str1 <=> str2 >= 0) {
        std::cerr << "Error: <=> does not work." << std::endl;
        return 1;
    }
    if (str2 <=> str1 <= 0) {
        std::cerr << "Error: <=> does not work." << std::endl;
        return 1;
    }
    
    std::cout << "PASSED!" << std::endl;
    return 0;
}