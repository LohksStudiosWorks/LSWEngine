#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>
#include <battery/embed.hpp>

using namespace LSWE;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    const auto music_embedded = b::embed<"resources/7. Continuity.ogg">();

    al_init();

    auto fp = Utility::File::open_mem((void*)music_embedded.data(), music_embedded.length(), "rb");

    auto voice = Audio::Voice::create();
    auto mixer = Audio::Mixer::create();

    auto sample = Audio::Sample::load(fp, ".ogg");
    auto instance = Audio::SampleInstance::create(sample);

    voice << mixer;
    mixer << instance;

    voice.set_playing(true);
    mixer.set_playing(true);
    instance.play();

    al_rest(10);
   

    return 0;
}