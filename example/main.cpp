#include <LSWE/lswe.hpp>
#include <battery/embed.hpp>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <iostream>

using namespace LSWE;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    constexpr float cube_side = 0.5f;
    constexpr size_t cube_len = 36;
    Graphics::Vertex cube[cube_len]; // aka ALLEGRO_VERTEX[]

    auto set_vert = [&](int index, float x, float y, float z, float u, float v) {
        cube[index].x = x; cube[index].y = y; cube[index].z = z;
        cube[index].u = u; cube[index].v = v; cube[index].color = al_map_rgb_f(1.0f, 1.0f, 1.0f); // tint
    };
    auto set_face = [&](size_t base_idx, 
                    float x1, float y1, float z1,
                    float x2, float y2, float z2,
                    float x3, float y3, float z3,
                    float x4, float y4, float z4) {
        // Triangle 1 (Vertices: 0, 1, 2)
        set_vert(base_idx + 0, x1, y1, z1, 0.0f,  0.0f);  // Top-Left
        set_vert(base_idx + 1, x2, y2, z2, 16.0f, 0.0f);  // Top-Right
        set_vert(base_idx + 2, x3, y3, z3, 16.0f, 16.0f); // Bottom-Right

        // Triangle 2 (Vertices: 0, 2, 3)
        set_vert(base_idx + 3, x1, y1, z1, 0.0f,  0.0f);  // Top-Left
        set_vert(base_idx + 4, x3, y3, z3, 16.0f, 16.0f); // Bottom-Right
        set_vert(base_idx + 5, x4, y4, z4, 0.0f,  16.0f); // Bottom-Left
    };
    {
        const float& s = cube_side;
        // 1. Front Face (z = 0.5)
        set_face(0,  -s,  s, s,   s,  s, s,   s, -s, s,  -s, -s, s);
        // 2. Back Face (z = -0.5)
        set_face(6,   s,  s, -s, -s,  s, -s,  -s,-s, -s,   s, -s, -s);
        // 3. Left Face (x = -0.5)
        set_face(12, -s,  s, -s, -s,  s,  s,  -s,-s,  s,  -s, -s, -s);
        // 4. Right Face (x = 0.5)
        set_face(18,  s,  s,  s,  s,  s, -s,   s,-s, -s,   s, -s,  s);
        // 5. Top Face (y = 0.5)
        set_face(24, -s,  s, -s,  s,  s, -s,   s, s,  s,  -s,  s,  s);
        // 6. Bottom Face (y = -0.5)
        set_face(30, -s, -s,  s,  s, -s,  s,   s,-s, -s,  -s, -s, -s);
    }

    std::cout << "Loading PNG data from header..." << std::endl;

    const auto png_embedded = b::embed<"resources/dirt_block.png">();
    auto fp = Utility::File::open_mem((void*)png_embedded.data(), png_embedded.length(), "rb");

    constexpr float multiplier_speed = 1.0f;

    const int width = 800;
    const int height = 600;

    std::cout << "Setting up display flags..." << std::endl;

    Graphics::Display::set_new_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    Graphics::Display::set_new_option(ALLEGRO_DEPTH_SIZE, 16, ALLEGRO_SUGGEST);
    Graphics::Display::set_new_option(ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST);

    std::cout << "Creating display..." << std::endl;

    Graphics::Display::set_new_window_title("Fancy test");
    auto display = Graphics::Display::create(width, height);
    auto font = Graphics::Font::create_builtin();

    std::cout << "Loading png..." << std::endl;

    auto bitmap = Graphics::Bitmap::load(fp, ".png");

    std::cout << "Setting up cursor and flags..." << std::endl;

    display.hide_cursor();
    display.set_render_state(ALLEGRO_DEPTH_TEST, 1);

    std::cout << "Setting up perspective and camera..." << std::endl;

    Game::PlayerCamera player(2.0f);
    Game::PlayerPerspective perspective(display);
    auto identity = Graphics::Transform().set_identity();
    
    //camera.use();
    player.teleport(0.0f, 0.0f, 0.0f);
    player.teleport_angle(-0.7854f, -0.2f);
    player.calc_apply_camera();
    perspective.set_fov(120.0f);
    perspective.build_and_apply();

    std::cout << "Loading vertex buffer..." << std::endl;

    auto vertexes = Graphics::Vertexes::create(cube, Graphics::Vertexes::type::TRIANGLE_LIST, 0, bitmap);

    Utility::SingletonOf<Utility::AllegroInitMouse>::instance().setup();
    Utility::SingletonOf<Utility::AllegroInitKeyboard>::instance().setup();

    auto evq = Events::EventQueue::create();
    auto timed_ev = Events::Timer::create(1.0 / 60);
    auto timed_fps_calc = Events::Timer::create(1.0 / 4);

    evq << display;
    evq << Events::MouseEventSource();
    evq << Events::KeyboardEventSource();
    evq << timed_ev;
    evq << timed_fps_calc;

    timed_ev.start();
    timed_fps_calc.start();

    std::cout << "Start." << std::endl;

    double fps = 0.0;
    double delta_smooth = 1.0;
    double last_draw = al_get_time() - 1.0 / 60;

    for(bool running = true; running;) {
        while (evq.get_is_empty()) {
            display.set_render_state(ALLEGRO_DEPTH_TEST, 1);
            perspective.build_and_apply();
            player.calc_apply_camera();

            Graphics::Color(15, 15, 20).clear_to_this();
            display.clear_depth_buffer(1.0f);

            vertexes->translate(5.0f, 0.0f, 0.0f);
            vertexes.draw();
            vertexes->translate(0.0f, 0.0f, 5.0f);
            vertexes.draw();
            vertexes->translate(-5.0f, 0.0f, 0.0f);
            vertexes.draw();
            vertexes->translate(-5.0f, 0.0f, 0.0f);
            vertexes.draw();
            vertexes->translate(0.0f, 0.0f, -5.0f);
            vertexes.draw();
            vertexes->translate(0.0f, 0.0f, -5.0f);
            vertexes.draw();
            vertexes->translate(5.0f, 0.0f, 0.0f);
            vertexes.draw();
            vertexes->translate(5.0f, 0.0f, 0.0f);
            vertexes.draw();
            vertexes->translate(-5.0f, 0.0f, 5.0f);

            const double now_s = al_get_time();
            const double delta_s = now_s - last_draw;
            last_draw = now_s;
            delta_smooth = (9.0 * delta_smooth + delta_s) * 0.1;
            
            display.set_render_state(ALLEGRO_DEPTH_TEST, 0);

            identity.set_identity().use();
            identity.orthographic(0.0f, 0.0f, -1.0f, 320.0f, 240.0f, 1.0f).use_projection();

            font.draw(al_map_rgb(255,255,255), 2, 2, 0, "%.1lf FPS", fps);

            display.flip();
        }

        ALLEGRO_EVENT ev = evq.wait();

        switch (ev.type) {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            running = false;
            break;
        case ALLEGRO_EVENT_TIMER:
        {
            if (ev.timer.source == timed_fps_calc) {
                fps = 1.0 / delta_smooth;
            }

            player.cpu_tick();

            Events::Keyboard key_state;

            const float prop = (0.018f + static_cast<float>(key_state.get_down(ALLEGRO_KEY_LSHIFT)) * 0.035f) * multiplier_speed;

            if (key_state.get_down(ALLEGRO_KEY_W)) {
                player.walk_towards(0.0f, prop);
            }
            if (key_state.get_down(ALLEGRO_KEY_S)) {
                player.walk_towards(0.0f, -prop);
            }
            if (key_state.get_down(ALLEGRO_KEY_A)) {
                player.walk_towards(-prop, 0.0f);
            }
            if (key_state.get_down(ALLEGRO_KEY_D)) {
                player.walk_towards(prop, 0.0f);
            }
            if (key_state.get_down(ALLEGRO_KEY_SPACE) && player.get_y() == 0) {
                player.jump(0.3f);
            }
        }
            break;
        case ALLEGRO_EVENT_KEY_UP:
            switch(ev.keyboard.keycode) {
            case ALLEGRO_KEY_ESCAPE:
                running = false;
                break;
            case ALLEGRO_KEY_F11:
                display.set_flag(ALLEGRO_FULLSCREEN_WINDOW, !(display.get_flags() & ALLEGRO_FULLSCREEN_WINDOW));
                display.acknowledge_resize();
                perspective.build_and_apply();
                vertexes = Graphics::Vertexes::create(cube, Graphics::Vertexes::type::TRIANGLE_LIST, 0, bitmap);
                break;
            case ALLEGRO_KEY_R:
                player.teleport(0.0f, 0.0f, 0.0f);
                player.teleport_angle(-0.7854f, -0.2f);
                break;
            }
            break;
        case ALLEGRO_EVENT_MOUSE_AXES:
            player.move_angle(- ev.mouse.dx * 0.35f, ev.mouse.dy * 0.35f);
            display.move_mouse(width / 2, height / 2);
            break;
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
            display.acknowledge_resize();
            perspective.build_and_apply();
            vertexes = Graphics::Vertexes::create(cube, Graphics::Vertexes::type::TRIANGLE_LIST, 0, bitmap);
            break;
        }
    }
    
    std::cout << "PASSED!" << std::endl;
    return 0;
}



