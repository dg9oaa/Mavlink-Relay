/* 
 * MIT License
 * 
 * Copyright (c) 2025 Jonny Roeker
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
 * File:   logging.h
 * Author: jonny
 *
 * Created on 30. Mai 2025
 */

#ifndef LOGGING_H
#define LOGGING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define LOGFILE_DEFAULT_NAME     "/tmp/mavrpt.log"
#define LOGFILE_DEFAULT_SIZE     512 * 1024
#define LOGFILE_DEFAULT_SIZE_MAX 1024 * 1024 // default: 1 MB


    typedef enum {
        LogLevel_TRACE,
        LOGLEVEL_DEBUG,
        LOGLEVEL_INFO,
        LOGLEVEL_WARN,
        LOGLEVEL_ERROR,
        LOGLEVEL_NONE,
        LOGLEVEL_COUNT
    } LogLevel;

    /** set log level */
    void log_set_level(LogLevel level);
    void log_set_file(const char *filename, size_t max_size_bytes);
    void logSTD();
    const LogLevel loglevel_from_string(const char *string);
    const char* loglevel_to_string(LogLevel level);
    void log_close();
    void log_msg(LogLevel level, const char *fmt, ...);

#define LOG__TRACE(...) log_msg(LogLevel_TRACE, __VA_ARGS__)
#define LOG__DEBUG(...) log_msg(LOGLEVEL_DEBUG, __VA_ARGS__)
#define LOG__INFO(...) log_msg(LOGLEVEL_INFO,  __VA_ARGS__)
#define LOG__WARN(...) log_msg(LOGLEVEL_WARN,  __VA_ARGS__)
#define LOG__ERROR(...) log_msg(LOGLEVEL_ERROR, __VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif /* LOGGING_H */

