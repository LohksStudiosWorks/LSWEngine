#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>
#include <cmath>
#include <battery/embed.hpp>

using namespace LSWE;

void print_all_of(ALLEGRO_SAMPLE_INSTANCE* spl) {
    std::cout << "POS: " << al_get_sample_instance_position(spl)
        << "\nFRQ: " << al_get_sample_instance_frequency(spl) << std::endl;
}

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    const auto music_embedded = b::embed<"resources/7. Continuity.ogg">();

    al_init();

    auto fp = Utility::File::open_mem((void*)music_embedded.data(), music_embedded.length(), "rb");

    auto voice = Audio::Voice::create();
    auto mixer = Audio::Mixer::get_default();

    auto sample = Audio::Sample::load(fp, ".ogg");
    auto instance = Audio::SampleInstance::create(sample);

    //voice << mixer;
    mixer << instance;

    voice.set_playing(true);
    mixer.set_playing(true);
    
    instance.set_playing(true);

    al_rest(1.5f);

    instance.set_playing(false);

    if (fabs(1.5f - instance.get_position_seconds()) > 0.5f) {
        std::cerr << "Instance did not play for 1.5 seconds or so. It should've. Time played: " << instance.get_position_seconds() << std::endl;
        print_all_of(instance);
        return 1;
    }

    instance.set_playing(true);

    al_rest(1.5f);

    instance.set_playing(false);

    if (fabs(3.0f - instance.get_position_seconds()) > 0.5f) {
        std::cerr << "Instance did not play for 3 seconds or so. It should've. Time played: " << instance.get_position_seconds() << std::endl;
        print_all_of(instance);
        return 1;
    }
   
    std::cout << "PASSED!" << std::endl;
    return 0;
}