#include <LSWE/lswe.hpp>
#include <iostream>
#include <cstdint>

using namespace LSWE;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    auto menu_builder = Utility::MenuBuilder().set_submenus({
        Utility::MenuBuilder().set_caption("First!").set_submenus({
            Utility::MenuBuilder().set_caption("&Exit").set_id(2),
            Utility::MenuBuilder(),
            Utility::MenuBuilder().set_caption("Spawn &Popup").set_id(3)
        }),
        Utility::MenuBuilder().set_caption("Second!").set_id(1),
        Utility::MenuBuilder().set_caption("Third!").set_id(4),
        Utility::MenuBuilder().set_caption("SubMenu").set_submenus({
            Utility::MenuBuilder().set_caption("Submenuu yaaa").set_id(50),
            Utility::MenuBuilder().set_caption("Submenuu yaaa 2").set_id(51),
            Utility::MenuBuilder().set_caption("Submenuu yaaa 3").set_id(52),
            Utility::MenuBuilder(),
            Utility::MenuBuilder().set_caption("Submenuu yaaa 4").set_id(53),
            Utility::MenuBuilder().set_caption("Self remove").set_id(100),
            Utility::MenuBuilder().set_caption("Submenuu yaaa 5").set_id(54),
            Utility::MenuBuilder().set_caption("Submenuu yaaa 6").set_id(55),
            Utility::MenuBuilder(),
            Utility::MenuBuilder().set_caption("Submenuu yaaa 7").set_id(56),
            Utility::MenuBuilder().set_caption("Self remove 2").set_id(101),
            Utility::MenuBuilder().set_caption("Self remove 3").set_id(102),
            Utility::MenuBuilder().set_caption("Submenuu yaaa 8").set_id(57),
        })
    });

    std::cout << "MENU BUILDER JSON:\n" << menu_builder.dump() << std::endl;

    auto menu = Utility::Menu::build_from(Utility::Menu::type::BAR, menu_builder);
    auto menu_popup = Utility::Menu::copy(menu, Utility::Menu::type::POPUP);
    

    auto disp = Graphics::Display::create(1280, 720);
    al_rest(1.0);
    
    disp << menu;    

    ALLEGRO_EVENT_QUEUE* ev_qu = al_create_event_queue();
    ALLEGRO_TIMER* tm = al_create_timer(10.0);
    al_register_event_source(ev_qu, disp);
    al_register_event_source(ev_qu, menu);
    al_register_event_source(ev_qu, menu_popup);
    al_register_event_source(ev_qu, al_get_timer_event_source(tm));

    al_start_timer(tm);
    
    while(1) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(ev_qu, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || ev.type == ALLEGRO_EVENT_TIMER)
            break;

        std::optional<Utility::Menu> opt_el = ev.user.source == menu
            ? menu.find_from_event(ev)
            : menu_popup.find_from_event(ev);

        if (ev.user.source == menu) {
            std::cout << "Event from BAR | ";
        }
        else {
            std::cout << "Event from POPUP | ";
        }

        const uint16_t id = ev.user.data1;
        
        std::cout << "Id: " << id << std::endl;

        if (opt_el.has_value()) {
            auto el = opt_el.value();

            std::cout << "Contains: " << el.get_caption() << "; " << el.get_id() << "; POS=" << el.get_pos() << std::endl;

            if (el.get_id() == 2)
                break;
            if (el.get_id() == 3) {
                al_rest(0.5);
                disp << menu_popup;
            }
                

            /*if (el.get_id() >= 100) {
                el.destroy_self();
            }
            else {*/
                el.set_caption(el.get_caption() + "!!");
            //}
        }
    }

    al_destroy_timer(tm);
    al_destroy_event_queue(ev_qu);

    return 0;
};