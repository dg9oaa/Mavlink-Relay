#include "serial.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

int open_serial(const char* device, int baudrate) {
    int fd = open(device, O_RDWR | O_NOCTTY);
    if (fd < 0) return -1;

    struct termios options;
    memset(&options, 0, sizeof(options));
    tcgetattr(fd, &options);

    cfsetispeed(&options, B57600);
    cfsetospeed(&options, B57600);

    options.c_cflag |= (CLOCAL | CREAD | CS8);
    options.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;

    tcsetattr(fd, TCSANOW, &options);
    return fd;
}

int read_serial(int fd, uint8_t* buf, int buf_size) {
    return read(fd, buf, buf_size);
}

void close_serial(int fd) {
    close(fd);
}

