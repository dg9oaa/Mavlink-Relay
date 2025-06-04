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
#include <stdbool.h>
#include <ctype.h>

static LogLevel current_level = LOGLEVEL_INFO;
static FILE *log_file = NULL;
static char log_filename[256] = LOGFILE_DEFAULT_NAME;
static size_t max_log_size = LOGFILE_DEFAULT_SIZE_MAX;
static bool log_to_stdout = false;

const char* loglevel_strings[] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "NONE"
};

extern char *progname;

void logSTD() {
    log_to_stdout = true;
}

/**
 * set the log level
 * @param level
 */
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

/**
 * returns LogLevel as string
 * @param level
 * @return 
 */
const char* loglevel_to_string(LogLevel level) {
    if (level >= 0 && level < LOGLEVEL_COUNT) {
        return loglevel_strings[level];
    }
    return "DEBUG"; //unknown";
}

/**
 * returns a LogLevel
 * @param string
 * @return LogLevel
 */
const LogLevel loglevel_from_string(const char* string) {
    char *str = strdup(string);
    char *s = str;
    while (*s) {
        *s = toupper((unsigned char) *s);
        s++;
    }
    for (int i = 0; i < LOGLEVEL_COUNT; i++) {
        if (strcmp(str, loglevel_strings[i]) == 0) {
            return (LogLevel) i;
        }
    }
    return LOGLEVEL_ERROR; // fallback
}




 /**
 * split the absolut path
  * 
 * @param fullpath
 * @param out_dir
 * @param dir_size
 * @param out_base
 * @param base_size
 * @param out_ext
 * @param ext_size
 */
static void split_path_components(const char* fullpath, char* out_dir, size_t dir_size,
        char* out_base, size_t base_size,
        char* out_ext, size_t ext_size) {

    const char* last_slash = strrchr(fullpath, '/');
    //if (!last_slash) last_slash = strrchr(fullpath, '\\');  // für Windows

    if (last_slash) {
        size_t path_len = last_slash - fullpath;
        if (path_len >= dir_size) path_len = dir_size - 1;
        strncpy(out_dir, fullpath, path_len);
        out_dir[path_len] = '\0';

        strncpy(out_base, last_slash + 1, base_size - 1);
        out_base[base_size - 1] = '\0';
    } else {
        // kein Verzeichnisteil enthalten
        strncpy(out_dir, ".", dir_size - 1);
        out_dir[dir_size - 1] = '\0';

        strncpy(out_base, fullpath, base_size - 1);
        out_base[base_size - 1] = '\0';
    }

    // Extension (falls vorhanden)
    char* dot = strrchr(out_base, '.');
    if (dot) {
        strncpy(out_ext, dot + 1, ext_size - 1);
        out_ext[ext_size - 1] = '\0';
        *dot = '\0'; // entferne die Extension vom Basename
    } else {
        out_ext[0] = '\0';
    }
}

static void check_logfile_size_and_rotate() {
    if (!log_file || log_file == stderr) return;

    struct stat st;
    if (stat(log_filename, &st) == 0 && st.st_size >= max_log_size) {
        fclose(log_file);

        char dir[256], base[256], ext[64];
        split_path_components(log_filename, dir, sizeof(dir), base, sizeof(base), ext, sizeof(ext));
        char old_logfile[256];
        sprintf(old_logfile, "%s/%s.old", dir, base);
        //printf("--neue datei:%s\n", old_logfile);

        rename(log_filename, old_logfile);
        log_file = fopen(log_filename, "w"); // neue Datei
    }
}

void log_msg(LogLevel level, const char *fmt, ...) {
    if (level < current_level) return;
check_logfile_size_and_rotate();
    FILE *out;
    if (!log_to_stdout) {
        check_logfile_size_and_rotate();

        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char timebuf[20];
        strftime(timebuf, sizeof (timebuf), "%Y-%m-%d %H:%M:%S", t);

        out = log_file ? log_file : stderr;
        fprintf(out, "[%s] %-5s: ", timebuf, loglevel_to_string(level));

    } else {
//printf("log->std|");
        out = stderr;
        fprintf(out, "%s [%-5s]: ", progname, loglevel_to_string(level));
    }


    va_list args;
    va_start(args, fmt);
    vfprintf(out, fmt, args);
    va_end(args);

    fprintf(out, "\n");
    fflush(out);
}

