/*
 * MIT License
 *
 * Copyright (c) 2026 Tommaso Bruno
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
 */

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "k4c/logging.h"

#define K4C_LOG_COLOR_RESET "\033[0m"
#define K4C_LOG_COLOR_DEBUG "\033[90m"
#define K4C_LOG_COLOR_INFO "\033[32m"
#define K4C_LOG_COLOR_WARN "\033[33m"
#define K4C_LOG_COLOR_ERROR "\033[31m"

#define K4C_LOG_TIMESTAMP_BUFFER_SIZE 32

typedef struct k4c_log_config {
    k4c_log_level min_level;
    k4c_log_timestamp timestamp;
    const char *message_prefix;
    const char *error_prefix;
    bool use_color;
} k4c_log_config;

static k4c_log_config config = {
    .min_level = K4C_LOG_LEVEL_DEBUG,
    .timestamp = K4C_LOG_TIMESTAMP_NONE,
    .message_prefix = "k4c",
    .error_prefix = "k4c",
    .use_color = true,
};

static const char *log_level_name(k4c_log_level level) {
    switch (level) {
        case K4C_LOG_LEVEL_DEBUG:
            return "DEBUG";
        case K4C_LOG_LEVEL_INFO:
            return "INFO";
        case K4C_LOG_LEVEL_WARN:
            return "WARN";
        case K4C_LOG_LEVEL_ERROR:
            return "ERROR";
    }

    return "UNKNOWN";
}

static const char *log_level_color(k4c_log_level level) {
    switch (level) {
        case K4C_LOG_LEVEL_DEBUG:
            return K4C_LOG_COLOR_DEBUG;
        case K4C_LOG_LEVEL_INFO:
            return K4C_LOG_COLOR_INFO;
        case K4C_LOG_LEVEL_WARN:
            return K4C_LOG_COLOR_WARN;
        case K4C_LOG_LEVEL_ERROR:
            return K4C_LOG_COLOR_ERROR;
    }

    return K4C_LOG_COLOR_RESET;
}

static void k4c_log_write_timestamp(k4c_log_timestamp timestamp) {
    char buffer[K4C_LOG_TIMESTAMP_BUFFER_SIZE];

    if (timestamp == K4C_LOG_TIMESTAMP_NONE) {
        return;
    }

    time_t now = time(NULL);
    if (timestamp == LOG_TIMESTAMP_UNIX) {
        printf("[%lld] ", (long long)now);
        return;
    }

    struct tm *local = localtime(&now);
    if (local == NULL) {
        return;
    }

    const char *format = timestamp == LOG_TIMESTAMP_TIME ? "%H:%M:%S" : "%Y-%m-%d %H:%M:%S";
    if (strftime(buffer, sizeof(buffer), format, local) > 0) {
        printf("[%s] ", buffer);
    }
}

static void k4c_internal_log(k4c_log_level level, const char *fmt, va_list args) {
    if (level < config.min_level) {
        return;
    }

    const char *prefix = level == K4C_LOG_LEVEL_ERROR ? config.error_prefix : config.message_prefix;
    const char *level_name = log_level_name(level);

    k4c_log_write_timestamp(config.timestamp);
    if (prefix != NULL && prefix[0] != '\0') {
        printf("%s ", prefix);
    }
    if (config.use_color) {
        printf("%s[%s]%s ", log_level_color(level), level_name, K4C_LOG_COLOR_RESET);
    } else {
        printf("[%s] ", level_name);
    }

    vprintf(fmt, args);
    printf("\n");
}

void k4c_log_set_level(k4c_log_level level) {
    config.min_level = level;
}

void k4c_log_set_timestamp(k4c_log_timestamp timestamp) {
    config.timestamp = timestamp;
}

void k4c_log_set_prefixes(const char *message_prefix, const char *error_prefix) {
    config.message_prefix = message_prefix;
    config.error_prefix = error_prefix;
}

void k4c_log_set_color(bool enabled) {
    config.use_color = enabled;
}

void k4c_log_debug(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    k4c_internal_log(K4C_LOG_LEVEL_DEBUG, fmt, args);
    va_end(args);
}

void k4c_log_info(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    k4c_internal_log(K4C_LOG_LEVEL_INFO, fmt, args);
    va_end(args);
}

void k4c_log_warn(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    k4c_internal_log(K4C_LOG_LEVEL_WARN, fmt, args);
    va_end(args);
}

void k4c_log_error(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    k4c_internal_log(K4C_LOG_LEVEL_ERROR, fmt, args);
    va_end(args);
}
