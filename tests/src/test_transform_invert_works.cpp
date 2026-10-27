#include <LSWE/lswe.hpp>
#include <iostream>
#include <cmath>

using namespace LSWE;

int main(int argc, char *argv[]) {
    const std::string platform = (argc > 1) ? argv[1] : "Linux";
    std::cout << "Running test on " << platform << "..." << std::endl;

    constexpr float epsilon = 1.0e-7f;

    auto random_transform = Graphics::Transform()
        .rotate(0.7f, 0.3f, 0.1f, 1.2345f)
        .translate(235.0f, 15.4326f, 99.99f)
        .rotate(0.3f, 0.9f, 0.65f, 0.994f)
        .scale(2.346f, 66.43f, 12.345f)
        .translate(9.25f, 0.888f, 13.45f)
        .rotate(0.1f, 0.553f, 0.95f, 1.711f);

    auto inverse_test = random_transform.clone();
    inverse_test.invert_3d();

    auto composed = random_transform.clone();
    composed.compose(inverse_test);

    if (composed.check_inverse_3d(epsilon) != 1) {
        std::cerr << "FAILED!" << std::endl;
        return 1;
    }

    std::cout << "PASSED!" << std::endl;
    return 0;
}