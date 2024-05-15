
#include "sample_image.hpp"

#include <fstream>

int main() {
    std::ofstream out("image.ppm");
    write_sample_image(out);
    return 0;
}
