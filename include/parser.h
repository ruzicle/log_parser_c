#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

/* Represents a string without copying it */
typedef struct {
    const char *data;
    size_t      len;
} StringView;

/* Holds the parsed parts of a single log line */
typedef struct {
    StringView ip;
    StringView timestamp;
    StringView method;
    StringView endpoint;
    int        status_code;
    long       response_size;
} LogRecord;

/* Parses a line into the record struct. Returns 0 on success, -1 on error. */
int parse_log_line(const char *line, LogRecord *record);

#endif /* PARSER_H */
