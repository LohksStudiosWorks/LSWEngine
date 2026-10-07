#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>

static uint32_t times_created_for_test{};

class Test {
public:
    Test() {
        ++times_created_for_test;
    }

    ~Test() {
        --times_created_for_test;
    }

    void hello_world() const {
        std::cout << "Hello test!" << std::endl;
    }
};

class DummyTest{};

using namespace LSWE::Utility;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;
    
    if (times_created_for_test != 0) {
        std::cerr << "Init of app did not start zeroed." << std::endl;
        return 1;
    }

    {
        SingletonOf<Test> testSingleton;

        if (times_created_for_test != 0) {
            std::cerr << "Instancing Singleton class shouldn't cause singleton to exist yet." << std::endl;
            return 1;
        }
        
        SingletonOf<Test>::instance().hello_world();

        if (times_created_for_test != 1) {
            std::cerr << "Singleton did not behave as expected." << std::endl;
            return 1;
        }

        testSingleton->hello_world();

        if (times_created_for_test != 1) {
            std::cerr << "Singleton did not behave as expected." << std::endl;
            return 1;
        }


        if (SingletonInfo::list_all().size() != 1) {
            std::cerr << "Singleton info was not properly updated." << std::endl;
            return 1;
        }
        
        auto& __dum = SingletonOf<DummyTest>::instance();

        if (SingletonInfo::list_all().size() != 2) {
            std::cerr << "Singleton info was not properly updated." << std::endl;
            return 1;
        }

        std::cout << "Fun fact: instanced singletons:" << std::endl;
        for(const auto& i : SingletonInfo::list_all()) {
            std::cout << "- " << i << std::endl;
        }

    }
    if (times_created_for_test != 1) {
        std::cerr << "Singleton was destroyed when it shouldn't." << std::endl;
        return 1;
    }

    std::cout << "PASSED!" << std::endl;

    return 0;
}
