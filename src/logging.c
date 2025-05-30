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
 * 
 * File:   logging.c
 * Author: jonny
 *
 * Created on 30. Mai 2025
 */

#include "logging.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

static LogLevel current_level = INFO;
static FILE *log_file = NULL;
static char log_filename[256] = "log.txt";
static size_t max_log_size = 1024 * 1024; // default: 1 MB

void log_set_level(LogLevel level) {
    current_level = level;
}

void log_set_file(const char *filename, size_t max_size_bytes) {
    strncpy(log_filename, filename, sizeof (log_filename) - 1);
    max_log_size = max_size_bytes;
    log_file = fopen(log_filename, "a");
    if (!log_file) {
        perror("log file open");
        log_file = stderr; // Fallback
    }
}

void log_close() {
    if (log_file && log_file != stderr) {
        fclose(log_file);
        log_file = NULL;
    }
}

static const char *level_to_string(LogLevel level) {
    switch (level) {
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case WARN: return "WARN";
        case ERROR: return "ERROR";
        default: return "LOG";
    }
}

static void check_logfile_size_and_rotate() {
    if (!log_file || log_file == stderr) return;

    struct stat st;
    if (stat(log_filename, &st) == 0 && st.st_size >= max_log_size) {
        fclose(log_file);
        rename(log_filename, "log.old");
        log_file = fopen(log_filename, "w"); // neue Datei
    }
}

void log_msg(LogLevel level, const char *fmt, ...) {
    if (level < current_level) return;

    check_logfile_size_and_rotate();

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timebuf[20];
    strftime(timebuf, sizeof (timebuf), "%Y-%m-%d %H:%M:%S", t);

    FILE *out = log_file ? log_file : stderr;
    fprintf(out, "[%s] %-5s: ", timebuf, level_to_string(level));

    va_list args;
    va_start(args, fmt);
    vfprintf(out, fmt, args);
    va_end(args);

    fprintf(out, "\n");
    fflush(out);
}
