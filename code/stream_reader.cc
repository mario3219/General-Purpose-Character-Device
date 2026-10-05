#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <poll.h>
#include <unistd.h>

int main()
{
    const char* device = "/dev/streamdev0";

    int fd = open(device, O_RDONLY | O_NONBLOCK);

    if (fd < 0) {
        std::cerr
            << "open(): "
            << std::strerror(errno)
            << '\n';

        return 1;
    }

    std::cout << "Opened " << device << '\n';

    pollfd pfd{};

    pfd.fd = fd;
    pfd.events = POLLIN;

    while (true) {

        int ret = poll(&pfd, 1, 5000);

        if (ret < 0) {
            if (errno == EINTR)
                continue;
            std::cerr
                << "poll(): "
                << std::strerror(errno)
                << '\n';
            break;
        }
        if (ret == 0) {
            std::cout << "poll timeout\n";
            continue;
        }
        if (pfd.revents & POLLIN) {
            uint32_t samples[32];
            ssize_t bytes = read(fd, samples, sizeof(samples));

            if (bytes < 0) {
                if (errno == EAGAIN)
                    continue;
                std::cerr
                    << "read(): "
                    << std::strerror(errno)
                    << '\n';
                break;
            }

            std::size_t count = bytes / sizeof(uint32_t);

            for (std::size_t i = 0; i < count; ++i) {
                std::cout
                    << "sample: "
                    << samples[i]
                    << '\n';
            }
        }

        if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
            std::cerr << "device error\n";
            break;
        }
    }
    close(fd);

    return 0;
}
