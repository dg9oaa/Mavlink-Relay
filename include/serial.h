#pragma once
#include <stdint.h>
#include <stdbool.h>

int openSerial(const char* device, int baudrate);
int readSerial(int fd, uint8_t* buffer, int buffer_size);
int writeSerial(int fd, uint8_t* buffer, int len);
void closeSerial(int fd);
void statusSerial();

