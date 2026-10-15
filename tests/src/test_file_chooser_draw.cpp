#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>
#include <future>
#include <chrono>

using namespace LSWE;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    Graphics::MonitorInfo moninfo;

    auto test_select = std::async(std::launch::async, [] {
        auto file_chooser = Utility::FileChooser::create(
            "Select an image to render",
            "image/jpg;*.jpg;image/png;*.png",
            "",
            ALLEGRO_FILECHOOSER_FILE_MUST_EXIST | ALLEGRO_FILECHOOSER_PICTURES);

        std::vector<std::string> list;

        while(list.size() == 0) 
            list = file_chooser.request();

        return list;
    });

    for(double noww = al_get_time() + 5.0; noww - al_get_time() > 0.0;) {
        if (test_select.wait_for(std::chrono::milliseconds(100)) == std::future_status::ready)
            break;
        al_rest(1.0);
    }

    if (test_select.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
        std::cout << "Dropping: no interaction." << std::endl;
        std::exit(0);
    }

    auto list = test_select.get();

    Utility::create_message_box("About to load the following!", "This is the path selected:", list[0].c_str(), 0, "YAY|YOO|YAHOO|YEE");

    Graphics::Display::set_new_window_title("Fancy test");
    auto display = Graphics::Display::create(moninfo.get_width() * 0.75f, moninfo.get_height() * 0.75f);
    auto bitmap = Graphics::Bitmap::load(list[0].c_str());

    const float scales[2] = {
        1.0f * display.get_width() / bitmap.get_width(),
        1.0f * display.get_height() / bitmap.get_height(),
    };
    const float min = std::min(scales[0], scales[1]);

    display->clear(al_map_rgb(50, 50, 50));

    bitmap.draw(bitmap.get_width() / 2, bitmap.get_height() / 2, display.get_width() / 2, display.get_height() / 2,
        0.8f * min, 0.8f * min, 0.0f, 0);
    
    display.flip();

    al_rest(5.0);    

    std::cout << "PASSED!" << std::endl;
    return 0;
}