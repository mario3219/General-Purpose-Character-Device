#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main()
{
    int fd = open("/dev/ecg0", O_RDONLY);

    if (fd < 0) {
        perror("open");
        return 1;
    }

    int16_t sample;

    ssize_t bytes_read = read(fd, &sample, sizeof(sample));

    if (bytes_read < 0) {
        perror("read");
        close(fd);
        return 1;
    }

    std::cout << "Bytes read: " << bytes_read << '\n';
    std::cout << "ECG sample: " << sample << '\n';

    close(fd);

    return 0;
}
