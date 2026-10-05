#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

int main() {

    const std::string device_path = "/dev/streamdev0";
    const std::string input_path = "/data.raw";
    const double sample_rate = 10;

    if (sample_rate <= 0.0) {
        std::cerr << "Sample rate must be greater than 0\n";
        return 1;
    }

    /*
     * Open the driver.
     */
    int fd = open(device_path.c_str(), O_WRONLY);

    if (fd < 0) {
        std::cerr
            << "Failed to open " << device_path
            << ": " << std::strerror(errno) << '\n';

        return 1;
    }

    std::ifstream input(input_path, std::ios::binary);

    if (!input) {
        std::cerr << "Failed to open " << input_path << '\n';
        close(fd);
        return 1;
    }

    const auto sample_period =
        std::chrono::duration<double>(1.0 / sample_rate);

    int16_t sample;

    while (input.read(
        reinterpret_cast<char*>(&sample),
        sizeof(sample)))
    {
        ssize_t written = write(
            fd,
            &sample,
            sizeof(sample)
        );

        if (written < 0) {
            std::cerr
                << "write failed: "
                << std::strerror(errno)
                << '\n';

            break;
        }

        if (written != sizeof(sample)) {
            std::cerr
                << "Partial write: "
                << written
                << " / "
                << sizeof(sample)
                << " bytes\n";
        }

        std::this_thread::sleep_for(sample_period);
    }

    close(fd);

    std::cout << "Finished streaming data\n";

    return 0;
}
