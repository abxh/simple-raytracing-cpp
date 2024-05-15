
#include "sample_image.hpp"
#include "color.hpp"

#include <iostream>

// relevant source:
// https://raytracing.github.io/books/RayTracingInOneWeekend.html#outputanimage

void write_sample_image(std::ostream& out) {
    int image_width = 256;
    int image_height = 256;

    out << "P3" << "\n";
    out << image_width << " " << image_height << "\n";
    out << "255" << "\n";

    for (int j = 0; j < image_height; j++) {
        std::clog << "\r" << "Scanlines left: " << (image_height - j) << " " << std::flush;

        for (int i = 0; i < image_width; i++) {
            auto r = static_cast<double>(i) / (image_width - 1);
            auto g = static_cast<double>(j) / (image_height - 1);
            auto b = 0.0;

            auto pixel_color = color{r, g, b};

            write_color(out, pixel_color);
        }
    }

    std::clog << "\r" << "Done." << std::string(sizeof("Scanlines left: 100"), ' ') << "\n";
}
