#ifndef ANALYZER_H
#define ANALYZER_H

#include "parser.h"

#define MAX_TRACKED_ITEMS 2048
#define MAX_ENDPOINT_LEN 64
#define MAX_IP_LEN 32

typedef struct {
    char endpoint[MAX_ENDPOINT_LEN]; /* Physical copy of the endpoint */
    int hits;
    int errors;
    long total_bytes;
} EndpointStat;

typedef struct {
    char ip[MAX_IP_LEN]; /* Physical copy of the IP address */
    int hits;
} IpStat;

typedef struct {
    EndpointStat endpoints[MAX_TRACKED_ITEMS];
    int endpoint_count;

    IpStat ips[MAX_TRACKED_ITEMS];
    int ip_count;

    int total_requests;
    int status_2xx;
    int status_3xx;
    int status_4xx;
    int status_5xx;
} AnalyzerContext;

/* Initializes the context with zeros */
void analyzer_init(AnalyzerContext *ctx);

/* Processes a single parsed record and updates statistics */
void analyzer_process_record(AnalyzerContext *ctx, const LogRecord *record);

#endif /* ANALYZER_H */
