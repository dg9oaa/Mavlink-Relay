#pragma once
#include <stdint.h>

int open_serial(const char* device, int baudrate);
int read_serial(int fd, uint8_t* buf, int buf_size);
void close_serial(int fd);

