#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>
#include <battery/embed.hpp>

using namespace LSWE;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    const auto cfg_embedded = b::embed<"resources/config.conf">();

    auto fp = Utility::File::open_mem((void*)cfg_embedded.data(), cfg_embedded.length(), "rb");    

    auto memcfg = Utility::Config::load(fp);
    auto cfg = Utility::Config::create();

    cfg.add_section("general", "This is a general purpose config section");
    cfg.set("general", "key1", "1");
    cfg.set("general", "key2", "2");
    cfg.set("general", "key3", "3");
    cfg.set("general", "key4", "4");
    cfg.set("general", "key5", "5");
    cfg.add_section("", "This should be the root global config");
    cfg.set("", "global1", "one");
    cfg.set("", "global2", "two");
    cfg.set("", "global3", "three");
    cfg.set("", "global4", "four");

    std::cout << "Testing original vs generated:" << std::endl;

    for(const auto& section : memcfg.get_all_sections()) {
        for(const auto& key : memcfg.get_all_entries(section.c_str())) {
            const char* original = memcfg.get(section.c_str(), key.c_str());
            const char* in_mem = cfg.get(section.c_str(), key.c_str());
            const bool is_eq = strcmp(original, in_mem) == 0;

            std::cout 
                << "[" << section << "]" 
                << "[" << key << "]" << ": "
                << original << " | " << in_mem
                << " ? " << (is_eq ? "MATCH" : "FAIL") << std::endl;

            if (!is_eq) return 1;
        }
    }

    std::cout << "Testing generated vs original:" << std::endl;

    for(const auto& section : cfg.get_all_sections()) {
        for(const auto& key : cfg.get_all_entries(section.c_str())) {
            const char* original = cfg.get(section.c_str(), key.c_str());
            const char* in_mem = memcfg.get(section.c_str(), key.c_str());
            const bool is_eq = strcmp(original, in_mem) == 0;

            std::cout 
                << "[" << section << "]" 
                << "[" << key << "]" << ": "
                << original << " | " << in_mem
                << " ? " << (is_eq ? "MATCH" : "FAIL") << std::endl;

            if (!is_eq) return 1;
        }
    }
    
    
    std::cout << "PASSED!" << std::endl;
    return 0;
}