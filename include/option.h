#pragma once

#include <stdbool.h>

typedef struct __options_t {
   bool  daemon;
   bool  verbose;
   char  *device;
   int   baud;
} options_t;

void parseOptions(int argc, char *argv[]);
void printUsage();
void getProgramName(char *argv[]);
