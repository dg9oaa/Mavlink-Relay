/* 
 * MIT License
 * 
 * Copyright (c) 2025 Jonny Röker
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * File:   option.c
 * Author: jonny
 *
 * Created on 29. Mai 2025, 14:26
 */


#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "option.h"
extern options_t options;
char *progname;

#define FILESEPERATOR '/'


static struct option long_options[] = {
    {"help",     no_argument,       0, 'h'},
    {"daemon",   no_argument,       0, 'D'},

    {"device",   required_argument, 0, 'd'},
    {"baudrate", required_argument, 0, 'b'},

    {"port",     required_argument, 0, 'p'},
    {0, 0, 0, 0}
};

void parseOptions(int argc, char *argv[]) {
    int opt = 0;
    int long_index = 0;

    while ((opt = getopt_long_only(argc, argv, "", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'h':
                printUsage();
                break;

            case 'D':
                options.daemon = true;
                printf("run as daemon\n");
                break;

            case 'd':
                options.device = optarg;
                break;

            case 'b':
                options.baud = atoi(optarg);
                break;

            case 'p':
                break;

            default:
                printUsage();
                break;
        }
    }
}

/**
 * get the program name
 * @param argv
 */
void getProgramName(char *argv[]) {
    char *prognamep;
    
    if ((prognamep = strrchr(argv[0], FILESEPERATOR)) == NULL)
        progname = argv[0];
    else {
        ++prognamep;
        progname = prognamep;
    }
}

void printUsage() {
    printf("Usage: \n");
    exit(EXIT_FAILURE);
}
