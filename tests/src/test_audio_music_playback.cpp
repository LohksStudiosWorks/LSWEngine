#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>
#include <battery/embed.hpp>

#include <allegro5/allegro_memfile.h>

using namespace LSWE::Audio;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    const auto music_embedded = b::embed<"resources/7. Continuity.ogg">();

    al_init();

    ALLEGRO_FILE* fp = al_open_memfile((void*)music_embedded.data(), music_embedded.length(), "rb");
    if (!fp) {
        std::cerr << "Could not load mem file in mem" << std::endl;
        return 1;
    }

    Voice voice = Voice::create();
    Mixer mixer = Mixer::create();

    Sample sample = Sample::load(fp, ".ogg");
    SampleInstance instance = SampleInstance::create(sample);

    voice << mixer;
    mixer << instance;

    voice.set_playing(true);
    mixer.set_playing(true);
    instance.play();

    al_rest(10);
   

    return 0;
}