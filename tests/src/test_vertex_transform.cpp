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

    al_init();
    al_install_keyboard();
    al_init_primitives_addon();

    const auto png_embedded = b::embed<"resources/dirt_block.png">();
    auto fp = Utility::File::open_mem((void*)png_embedded.data(), png_embedded.length(), "rb");


    Graphics::MonitorInfo moninfo;
    const int width = moninfo.get_width() * 0.75f;
    const int height = moninfo.get_height() * 0.75f;

    const float fov_rad = 60.0f * ALLEGRO_PI / 180.0f;
    const float fov_factor = tan(fov_rad / 2.0f);
    const float aspect = static_cast<float>(height) / static_cast<float>(width);

    Graphics::Display::set_new_flags(ALLEGRO_WINDOWED);
    Graphics::Display::set_new_option(ALLEGRO_DEPTH_SIZE, 16, ALLEGRO_SUGGEST);

    Graphics::Display::set_new_window_title("Fancy test");
    auto display = Graphics::Display::create(width, height);
    auto bitmap = Graphics::Bitmap::load(fp, ".png");

    display.set_render_state(ALLEGRO_DEPTH_TEST, 1);

    auto perspective = Graphics::Transform().build_perspective(
        -1.0f,               // left
        aspect,              // top
        1.0f / fov_factor,   // near (z-distance)
        1.0f,                // right
        -aspect,             // bottom
        100.0f               // far
    );
    auto camera = Graphics::Transform().build_camera(
        3.0f, 2.0f, 3.0f, // Camera Position
        0.0f, 0.0f, 0.0f, // Look-At Target (Center of the cube)
        0.0f, 1.0f, 0.0f  // Up Vector
    );
    perspective.use_projection();
    camera.use();

    // this builds internally ALLEGRO_VERTEX_BUFFER and stores the cube size
    auto vertexes = Graphics::Vertexes::create(cube, Graphics::Vertexes::type::TRIANGLE_LIST, 0, bitmap);


    al_clear_to_color(al_map_rgb(15, 15, 20));
    al_clear_depth_buffer(1.0f); // Clear the z-buffer

    vertexes.draw();

    display.flip();

    Utility::SingletonOf<Utility::AllegroInitMouse>::instance().setup();
    ALLEGRO_EVENT_QUEUE* evq = al_create_event_queue();

    al_register_event_source(evq, display);
    al_register_event_source(evq, al_get_mouse_event_source());

    while(1) {
        ALLEGRO_EVENT ev;
        if (!al_wait_for_event_timed(evq, &ev, 3.0))
            break;
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;
    }
    
    al_destroy_event_queue(evq);

    std::cout << "PASSED!" << std::endl;
    return 0;
}



