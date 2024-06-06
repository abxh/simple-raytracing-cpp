
#include "camera.hpp"
#include "common.hpp"
#include "hittable_list.hpp"
#include "ray.hpp"
#include "sphere.hpp"

int main() {
    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    auto aspect_ratio = 16.0 / 9.0;
    auto image_width = 400;
    auto samples_per_pixels = 100;
    camera cam{aspect_ratio, image_width, samples_per_pixels};

    std::ofstream out{"image.ppm"};

    cam.render(world, out);

    return 0;
}
