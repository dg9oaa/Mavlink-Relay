
//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "option.h"

extern options_t options;

static struct option long_options[] = {
        {"help",      no_argument,       0,  'h' },
        {"daemon",    no_argument,       0,  'D' },
        {"device",    required_argument, 0,  'd' },
        {"port",      required_argument, 0,  'p' },
        {0,           0,                 0,  0   }
    };

void parseoptions(int argc, char *argv[]) {
   int opt= 0;
   int long_index =0;

   while ((opt = getopt_long_only(argc, argv,"", long_options, &long_index )) != -1) {
      switch (opt) {
         case 'h':
            print_usage();
            break;

         case 'D':
            options.daemon = true;
            printf("run as daemon\n");
            break;

         case 'd':
            options.device = optarg;
            break;

         case 'p':
            break;

         default:
            print_usage();
            break;
      }
   }
}

void print_usage() {
    printf("Usage: \n");
    exit(EXIT_FAILURE);
}
