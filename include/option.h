#pragma once

#include <stdbool.h>

typedef struct __options_t {
   bool  daemon;
   bool  verbose;
   char  *device;
} options_t;

void parseoptions(int argc, char *argv[]);
void print_usage();
