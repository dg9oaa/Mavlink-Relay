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
#include <ctype.h>

#include "option.h"
#include "logging.h"

extern options_t options;
char *progname;


#define FILESEPERATOR '/'


static struct option long_options[] = {
    {"help",      no_argument,       0, 'h'},
    {"daemon",    no_argument,       0, 'D'},
    {"loglevel",  required_argument, 0, 'L'},

    {"device",    required_argument, 0, 'd'},
    {"baudrate",  required_argument, 0, 'b'},

    {"server",    required_argument, 0, 's'},
    {"port",      required_argument, 0, 'p'},

    {"function",  required_argument, 0, 'f'},
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

            case 'L':
            {
                char *str = strdup(optarg);
                char *s = str;
                while (*s) {
                    *s = toupper((unsigned char) *s);
                    s++;
                }
                logSetLevel(logStringToLevel(str));
                options.level = str;
            }
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
    if (options.baud == 0)
        options.baud = options.baud_dflt;
    if (options.device == NULL)
        options.device = options.device_dflt;

    //printUsage();
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

    printf(
            "Usage: %s [OPTIONS]\n"
            "Where:\n"
            "  --device      Serial MAVLink device (%s by default)\n"
            "  --baudrate    Serial MAVLink baudrate (%d by default)\n"
            "  --server      Server address (%s by default)\n"
            "  --port        Server port (%d by default)\n"
            "  --loglevel    Setting the log level (%s by default)\n"
            "  --daemon      Runs the program in the background and detaches it from the input shell\n"
            "  --function    Program function (client, server, direct). Is actually controlled via the program name (mavrelayclient, mavrelayserver, mavrelay)\n"
            "  --help        Display this help\n"
            , progname, options.device_dflt, options.baud_dflt, options.server, options.port, options.level_dflt );
    
    exit(EXIT_FAILURE);
}
