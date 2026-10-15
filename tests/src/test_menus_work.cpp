#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>

using namespace LSWE;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    auto menu = Utility::Menu::create(Utility::Menu::type::BAR);
    auto& submenu = menu.interface();

    submenu.push(Utility::Menu::element::create_submenu("First").as_menu()
        .push(Utility::Menu::element::create_item("&Exit", 2))
        .push(Utility::Menu::element::create_div_item())
        .push(Utility::Menu::element::create_item("Spawn &Popup", 3))
    );
    submenu.push(Utility::Menu::element::create_item("Second", 1));
    submenu.push(Utility::Menu::element::create_item("Third", 4));
    submenu.push(
        Utility::Menu::element::create_submenu("SubMenu").as_menu()
            .push(Utility::Menu::element::create_item("Submenu yaaa", 6))
            .push(Utility::Menu::element::create_item("Submenu yaaa 2", 7))
            .push(Utility::Menu::element::create_item("Submenu yaaa 3", 8))
            .push(Utility::Menu::element::create_div_item())
            .push(Utility::Menu::element::create_item("Submenu yaaa 4", 9))
            .push(Utility::Menu::element::create_item("Submenu yaaa 5", 10))
            .push(Utility::Menu::element::create_item("Submenu yaaa 6", 11))
            .push(Utility::Menu::element::create_div_item())
            .push(Utility::Menu::element::create_item("Submenu yaaa 7", 12))
            .push(Utility::Menu::element::create_item("Self remove", 20))
            .push(Utility::Menu::element::create_item("Self remove 2", 21))
        );
    
    auto menu_popup = menu.clone_as(Utility::Menu::type::POPUP);

    auto disp = Graphics::Display::create(1280, 720);
    al_rest(1.0);
    
    disp << menu;    

    ALLEGRO_EVENT_QUEUE* ev_qu = al_create_event_queue();
    ALLEGRO_TIMER* tm = al_create_timer(10.0);
    al_register_event_source(ev_qu, disp);
    al_register_event_source(ev_qu, al_enable_menu_event_source(menu));
    al_register_event_source(ev_qu, al_get_timer_event_source(tm));

    al_start_timer(tm);
    
    while(1) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(ev_qu, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || ev.type == ALLEGRO_EVENT_TIMER)
            break;

        Utility::Menu::element* el = menu.find_by_event(ev);        
        const uint16_t id = ev.user.data1;
        
        std::cout << "Event: " << id << std::endl;

        if (el) {
            std::cout << "Contains: " << el->get_caption() << "; " << el->get_id() << "; POS=" << el->get_pos() << std::endl;

            if (el->get_id() == 2)
                break;
            if (el->get_id() == 3) {
                al_rest(0.5);
                disp << menu_popup;
            }
                

            if (el->get_id() >= 20) {
                el->destroy_self();
            }
            else {
                el->set_caption(el->get_caption() + "!!");
            }
        }
    }

    al_destroy_timer(tm);
    al_destroy_event_queue(ev_qu);

    return 0;
};