#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>
#include <battery/embed.hpp>

using namespace LSWE;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    const auto ogv_embedded = b::embed<"resources/video.ogv">();

    auto fp = Utility::File::open_mem((void*)ogv_embedded.data(), ogv_embedded.length(), "rb");

    Graphics::Display::set_new_window_title("Fancy test");
    auto display = Graphics::Display::create(1280, 720);
    auto font = Graphics::Font::create_builtin();
    auto voice = Audio::Voice::create();
    auto mixer = Audio::Mixer::get_default();

    display->clear(al_map_rgb(50, 50, 50));
    constexpr char loading_str[] = "Loading GIF, please wait...";
    const auto s_width = font.get_width(loading_str);
    const auto s_height = font.get_line_height();

    font.draw(al_map_rgb(235,235,235), (display.get_width() - s_width) / 2, (display.get_height() - s_height) / 2, 0, loading_str);
    display.flip();

    ALLEGRO_EVENT_QUEUE* evq = al_create_event_queue();

    auto video = Graphics::Video::open(std::move(fp));

    al_register_event_source(evq, video);
    al_register_event_source(evq, display);

    const float scales[2] = {
        1.0f * display.get_width() / video.get_width_scaled(),
        1.0f * display.get_height() / video.get_height_scaled(),
    };
    const float min = std::min(scales[0], scales[1]);

    video.start(mixer);
    if (!video.get_playing()) {
        std::cout << "Had to hit play." << std::endl;
        video.set_playing(true);
    }

    for(double noww = al_get_time() + 65.0; noww - al_get_time() > 0.0;) {
        ALLEGRO_EVENT ev;
        if (!al_wait_for_event_timed(evq, &ev, 0.1)) continue;

        switch(ev.type) {
        case ALLEGRO_EVENT_VIDEO_FRAME_SHOW:
            display->clear(al_map_rgb(50, 50, 50));

            video.draw(video.get_width_scaled() / 2, video.get_height_scaled() / 2, display.get_width() / 2, display.get_height() / 2,
                0.8f * min, 0.8f * min, 0.0f, 0);
            
            display.flip();
            break;
        case ALLEGRO_EVENT_VIDEO_FINISHED:
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            noww = 0.0;
            break;
        }
    }
    
    al_destroy_event_queue(evq);

    std::cout << "PASSED!" << std::endl;
    return 0;
}