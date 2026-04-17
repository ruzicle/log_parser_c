#include "parser.h"
#include <string.h>
#include <stdlib.h>

int parse_log_line(const char *line, LogRecord *record) {
    if (line == NULL || record == NULL) {
        return -1;
    }

    const char *p = line;
    const char *end;

    /* 1. IP Adresa */
    end = strchr(p, ' ');
    if (end == NULL) return -1;
    
    record->ip.data = p;
    record->ip.len  = (size_t)(end - p);
    p = end + 1;

    /* 2. Timestamp */
    p = strchr(p, '[');
    if (p == NULL) return -1;
    p++; 
    
    end = strchr(p, ']');
    if (end == NULL) return -1;
    
    record->timestamp.data = p;
    record->timestamp.len  = (size_t)(end - p);
    p = end + 1;

    /* 3. HTTP Request */
    p = strchr(p, '"');
    if (p == NULL) return -1;
    p++; 

    end = strchr(p, ' ');
    if (end == NULL) return -1;
    
    record->method.data = p;
    record->method.len  = (size_t)(end - p);
    p = end + 1;

    end = strchr(p, ' ');
    if (end == NULL) return -1;
    
    record->endpoint.data = p;
    record->endpoint.len  = (size_t)(end - p);

    p = strchr(end, '"');
    if (p == NULL) return -1;
    p++; 

    /* 4. Status Code */
    p += strspn(p, " "); 
    
    char *next_ptr;
    record->status_code = (int)strtol(p, &next_ptr, 10);
    if (p == next_ptr) return -1; 
    p = next_ptr;

    /* 5. Response Size */
    p += strspn(p, " ");
    
    if (*p == '-') {
        record->response_size = 0;
    } else {
        record->response_size = strtol(p, NULL, 10);
    }

    return 0;
}
