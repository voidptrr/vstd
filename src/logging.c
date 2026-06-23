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

#include "vstd/logging.h"

#define VS_LOG_COLOR_RESET "\033[0m"
#define VS_LOG_COLOR_DEBUG "\033[90m"
#define VS_LOG_COLOR_INFO "\033[32m"
#define VS_LOG_COLOR_WARN "\033[33m"
#define VS_LOG_COLOR_ERROR "\033[31m"

#define VS_LOG_TIMESTAMP_BUFFER_SIZE 32

typedef struct vs_log_config {
    vs_log_level min_level;
    vs_log_timestamp timestamp;
    const char *message_prefix;
    const char *error_prefix;
    bool use_color;
} vs_log_config;

static vs_log_config vs_config = {
    .min_level = VS_LOG_LEVEL_DEBUG,
    .timestamp = VS_LOG_TIMESTAMP_NONE,
    .message_prefix = "vstd",
    .error_prefix = "vstd",
    .use_color = true,
};

static const char *vs_log_level_name(vs_log_level level) {
    switch (level) {
        case VS_LOG_LEVEL_DEBUG:
            return "DEBUG";
        case VS_LOG_LEVEL_INFO:
            return "INFO";
        case VS_LOG_LEVEL_WARN:
            return "WARN";
        case VS_LOG_LEVEL_ERROR:
            return "ERROR";
    }

    return "UNKNOWN";
}

static const char *vs_log_level_color(vs_log_level level) {
    switch (level) {
        case VS_LOG_LEVEL_DEBUG:
            return VS_LOG_COLOR_DEBUG;
        case VS_LOG_LEVEL_INFO:
            return VS_LOG_COLOR_INFO;
        case VS_LOG_LEVEL_WARN:
            return VS_LOG_COLOR_WARN;
        case VS_LOG_LEVEL_ERROR:
            return VS_LOG_COLOR_ERROR;
    }

    return VS_LOG_COLOR_RESET;
}

static bool vs_log_level_enabled(vs_log_level level) {
    return level >= vs_config.min_level;
}

static void vs_log_write_timestamp(vs_log_timestamp timestamp) {
    time_t now;
    struct tm *local;
    char buffer[VS_LOG_TIMESTAMP_BUFFER_SIZE];
    const char *format;

    if (timestamp == VS_LOG_TIMESTAMP_NONE) {
        return;
    }

    now = time(NULL);
    if (timestamp == VS_LOG_TIMESTAMP_UNIX) {
        printf("[%lld] ", (long long)now);
        return;
    }

    local = localtime(&now);
    if (local == NULL) {
        return;
    }

    format = timestamp == VS_LOG_TIMESTAMP_TIME ? "%H:%M:%S" : "%Y-%m-%d %H:%M:%S";
    if (strftime(buffer, sizeof(buffer), format, local) > 0) {
        printf("[%s] ", buffer);
    }
}

void vs_log_set_level(vs_log_level level) {
    vs_config.min_level = level;
}

void vs_log_set_timestamp(vs_log_timestamp timestamp) {
    vs_config.timestamp = timestamp;
}

void vs_log_set_prefixes(const char *message_prefix, const char *error_prefix) {
    vs_config.message_prefix = message_prefix;
    vs_config.error_prefix = error_prefix;
}

void vs_log_set_color(bool enabled) {
    vs_config.use_color = enabled;
}

static void vs_logv(vs_log_level level, const char *fmt, va_list args) {
    const char *prefix;
    const char *level_name;

    if (!vs_log_level_enabled(level)) {
        return;
    }

    prefix = level == VS_LOG_LEVEL_ERROR ? vs_config.error_prefix : vs_config.message_prefix;
    level_name = vs_log_level_name(level);

    vs_log_write_timestamp(vs_config.timestamp);
    if (prefix != NULL && prefix[0] != '\0') {
        printf("%s ", prefix);
    }
    if (vs_config.use_color) {
        printf("%s[%s]%s ", vs_log_level_color(level), level_name, VS_LOG_COLOR_RESET);
    } else {
        printf("[%s] ", level_name);
    }

    vprintf(fmt, args);
    printf("\n");
}

void vs_log_debug(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vs_logv(VS_LOG_LEVEL_DEBUG, fmt, args);
    va_end(args);
}

void vs_log_info(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vs_logv(VS_LOG_LEVEL_INFO, fmt, args);
    va_end(args);
}

void vs_log_warn(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vs_logv(VS_LOG_LEVEL_WARN, fmt, args);
    va_end(args);
}

void vs_log_error(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vs_logv(VS_LOG_LEVEL_ERROR, fmt, args);
    va_end(args);
}
