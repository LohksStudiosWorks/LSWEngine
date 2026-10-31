#include <LSWE/lswe.hpp>
#include <battery/embed.hpp>

#include <iostream>

using namespace LSWE;

struct fps_counter {
    double fps = 0.0;
    double delta_smooth = 1.0;
    double last_draw = 0.0;

    void drew() {
        const double now_s = al_get_time();
        const double delta_s = now_s - last_draw;
        last_draw = now_s;
        delta_smooth = (9.0 * delta_smooth + delta_s) * 0.1;
    }
    double get() { return 1.0 / std::max(1e-9, delta_smooth); }
};

std::vector<Graphics::Vertex> generate_cube();

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    auto cube = generate_cube();

    const b::EmbedInternal::EmbeddedFile embedded[2]{
        b::embed<"resources/dirt_block.png">(),
        b::embed<"resources/cobble_block.png">()
    };

    Utility::File files[2]{
        Utility::File::open_mem((void*)embedded[0].data(), embedded[0].length(), "rb"),
        Utility::File::open_mem((void*)embedded[1].data(), embedded[1].length(), "rb")
    };

    Graphics::Display::set_new_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    Graphics::Display::set_new_option(ALLEGRO_DEPTH_SIZE, 16, ALLEGRO_SUGGEST);
    Graphics::Display::set_new_option(ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST);
    
    Graphics::Display::set_new_window_title("Fancy test");
    auto display = Graphics::Display::create(1280, 720);
    auto font = Graphics::Font::create_builtin();
    fps_counter fps;

    display.hide_cursor();
    display.set_render_state(ALLEGRO_DEPTH_TEST, 1);

    Graphics::Bitmap bitmaps[2]{
        Graphics::Bitmap::load(files[0], ".png"),
        Graphics::Bitmap::load(files[1], ".png")
    };
    
    Game::Camera3D main_camera(display);
    main_camera.set_position({0.0f, 0.05f, 0.0f});
    main_camera.modify_gravity() = {0.0f, -0.0135f, 0.0f};
    main_camera.set_offset_height(2.0f);
    main_camera.set_speed({0.0f, 0.0f, 0.0f});
    main_camera.set_angle(-0.7854f, -0.2f);
    main_camera.modify_momentum() = {0.8f, 0.9997f, 0.8f};
    main_camera.set_fov(120.0f);

    Game::Camera2D text_camera;
    text_camera.set_limits(0.0f, 0.0f, 863.0f, 480.0f);

    auto cubix = Game::Entity::create_static_entity(cube, bitmaps[0], ALLEGRO_PRIM_TRIANGLE_LIST);

    cubix.as_object().move([](Graphics::Transform& t){ t.scale(2.0f, 1.5f, 2.0f).translate(-3.0f, 0.0f, 0.0f); });

    auto rotated_cubix = cubix.clone_to_static_entity();
    auto pushable_cubix = cubix.clone_to_dynamic_entity();

    rotated_cubix.as_object().move([](Graphics::Transform& t){ 
        t.scale(10.0f, 10.0f, 10.0f)
         .rotate(1.0f, 0.0f, 0.0f, ALLEGRO_PI / 4)
         .rotate(0.0f, 1.0f, 0.0f, ALLEGRO_PI / 2)
         .rotate(1.0f, 0.0f, 0.0f, ALLEGRO_PI / 4)
         .translate(-20.0f, -7.5f, 0.0f);
    });

    pushable_cubix.as_dynamic_object().move([](Graphics::Transform& t){ 
        t.translate(15.0f, 0.0f, -5.0f);
    });
    pushable_cubix.as_dynamic_object().modify_momentum() = {0.9f, 0.0f, 0.9f};

    std::vector<Game::Entity> around_cubes;
    for(int x = 0; x < 3; ++x) {
        for(int z = 0; z < 3; ++z) {
            if (z == 1 && x == 1) continue;

            const float x_coord = 5.0f * (x - 1);
            const float z_coord = 5.0f * (z - 1);

            auto cloned = cubix.clone_to_static_entity();
            cloned.as_object().move([x_coord, z_coord](Graphics::Transform& t){
                t.translate(x_coord, 0.0f, z_coord);
            });
            around_cubes.push_back(std::move(cloned));
        }
    }

    auto evq = Events::EventQueue::create();
    auto timed_ev = Events::Timer::create(1.0 / 60);
    auto timed_close = Events::Timer::create(15.0);

    evq << display;
    evq << Events::MouseEventSource();
    evq << Events::KeyboardEventSource();
    evq << timed_ev;
    evq << timed_close;

    timed_ev.start();
    timed_close.start();

    for(bool running = true; running;) {
        while (evq.get_is_empty()) {
            display.set_render_state(ALLEGRO_DEPTH_TEST, 1);
            main_camera.use_all();

            Graphics::Color(15, 15, 20).clear_to_this();
            display.clear_depth_buffer(1.0f);

            cubix.draw();
            pushable_cubix.draw();
            rotated_cubix.draw();
            for(const auto& cubic : around_cubes) {
                cubic.draw();
                Game::WrapObjectDrawable wrap(cubic.as_object());
                wrap.draw();
            }

            fps.drew();
            
            display.set_render_state(ALLEGRO_DEPTH_TEST, 0);

            text_camera.use_all();

            const auto& obj = pushable_cubix.as_dynamic_object();

            font.draw(al_map_rgb(255,255,255), 2, 2, 0,
                "P: %04.1f %04.1f %04.1f S: %06.3f %06.3f %06.3f A: %06.1f %05.1f TPS %04.1f",
                main_camera.get_position().x, main_camera.get_position().y, main_camera.get_position().z,
                main_camera.get_direction().x, main_camera.get_direction().y, main_camera.get_direction().z,
                main_camera.get_angle().x * 180.0f / ALLEGRO_PI, main_camera.get_angle().y * 180.0f / ALLEGRO_PI,
                main_camera.get_tps()
            );
            font.draw(al_map_rgb(255,255,255), 2, 10, 0,
                "Obj: P: %04.1f %04.1f %04.1f S: %06.3f %06.3f %06.3f TPS %04.1f",
                obj.get_position().x, obj.get_position().y, obj.get_position().z,
                obj.get_direction().x, obj.get_direction().y, obj.get_direction().z,
                obj.get_tps()
            );
            font.draw(al_map_rgb(255,255,255), 2, 18, 0, "%06.1lf FPS", fps.get());

            display.flip();
        }

        ALLEGRO_EVENT ev = evq.wait();

        switch (ev.type) {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            running = false;
            break;
        case ALLEGRO_EVENT_TIMER:
        {
            if (ev.timer.source == timed_close) {
                running = false;
            }
            else {
                main_camera.tick();
                pushable_cubix.tick();

                auto& dyn_pushable = pushable_cubix.as_dynamic_object();

                cubix.as_object().work_collision(main_camera);
                rotated_cubix.as_object().work_collision(main_camera);
                for (auto& cubic : around_cubes)
                    cubic.as_object().work_collision(main_camera);


                    
                cubix.as_object().work_collision(dyn_pushable);
                rotated_cubix.as_object().work_collision(dyn_pushable);
                for (auto& cubic : around_cubes)
                    cubic.as_object().work_collision(dyn_pushable);



                dyn_pushable.work_collision(main_camera);


                Events::Keyboard key_state;
                const bool ducked = key_state.get_down(ALLEGRO_KEY_LCTRL);
                const bool running = key_state.get_down(ALLEGRO_KEY_LSHIFT);

                const float prop =
                    (ducked 
                        ? 0.006f
                        : (running
                            ? 0.04f
                            : 0.018f
                        )
                    );

                main_camera.set_offset_height(ducked ? 1.2f : 2.0f);

                if (key_state.get_down(ALLEGRO_KEY_W)) {
                    timed_close.restart();
                    main_camera.walk(0.0f, prop);
                }
                if (key_state.get_down(ALLEGRO_KEY_S)) {
                    timed_close.restart();
                    main_camera.walk(0.0f, -prop);
                }
                if (key_state.get_down(ALLEGRO_KEY_A)) {
                    timed_close.restart();
                    main_camera.walk(-prop, 0.0f);
                }
                if (key_state.get_down(ALLEGRO_KEY_D)) {
                    timed_close.restart();
                    main_camera.walk(prop, 0.0f);
                }
                if (main_camera.get_position().y <= 0.1f) {
                    main_camera.combine_position({0.0f, - (main_camera.get_position().y) + 0.1f, 0.0f});
                    auto speed = main_camera.get_direction();
                    speed.y = 0.0f;
                    main_camera.set_speed(speed);

                    if (key_state.get_down(ALLEGRO_KEY_SPACE)) {
                        timed_close.restart();
                        main_camera.combine_speed({0.0f, 0.23f, 0.0f});
                    }
                }
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
                main_camera.update_projection();
                cubix.recreate_vertex_buffer();
                timed_close.restart();
                break;
            case ALLEGRO_KEY_R:
                main_camera.set_position({0.0f, 0.05f, 0.0f});
                main_camera.set_speed({0.0f, 0.0f, 0.0f});
                main_camera.set_angle(-0.7854f, -0.2f);
                timed_close.restart();
                break;
            }
            break;
        case ALLEGRO_EVENT_MOUSE_AXES:
            main_camera.angle(- ev.mouse.dx * 0.0011f, ev.mouse.dy * 0.0011f);
            display.move_mouse(display.get_width() / 2, display.get_height() / 2);
            timed_close.restart();
            break;
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
            display.acknowledge_resize();
            main_camera.update_projection();
            cubix.recreate_vertex_buffer();
            break;
        }
    }
    
    std::cout << "PASSED!" << std::endl;
    return 0;
}

std::vector<Graphics::Vertex> generate_cube() {
    constexpr float cube_side = 1.0f;
    constexpr size_t cube_len = 36;
    auto cube = std::vector<Graphics::Vertex>(cube_len);

    auto set_vert = [&](int index, float x, float y, float z, float u, float v) {
        cube[index].x = x; cube[index].y = y; cube[index].z = z;
        cube[index].u = u; cube[index].v = v; cube[index].color = al_map_rgb_f(1.0f, 1.0f, 1.0f); // tint
    };
    auto set_face = [&](size_t base_idx, float x1, float y1, float z1,float x2, float y2, float z2,float x3, float y3, float z3,
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
        set_face(0,  0,  s, s,   s,  s, s,   s, 0, s,  0, 0, s);
        // 2. Back Face (z = -0.5)
        set_face(6,   s,  s, 0, 0,  s, 0,  0,0, 0,   s, 0, 0);
        // 3. Left Face (x = -0.5)
        set_face(12, 0,  s, 0, 0,  s,  s,  0,0,  s,  0, 0, 0);
        // 4. Right Face (x = 0.5)
        set_face(18,  s,  s,  s,  s,  s, 0,   s,0, 0,   s, 0,  s);
        // 5. Top Face (y = 0.5)
        set_face(24, 0,  s, 0,  s,  s, 0,   s, s,  s,  0,  s,  s);
        // 6. Bottom Face (y = -0.5)
        set_face(30, 0, 0,  s,  s, 0,  s,   s,0, 0,  0, 0, 0);
    }

    return cube;
}
