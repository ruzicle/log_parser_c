#include "analyzer.h"
#include <string.h>

void analyzer_init(AnalyzerContext *ctx) {
    if (ctx != NULL) {
        memset(ctx, 0, sizeof(AnalyzerContext));
    }
}

/* Helper function to compare a standard C-string with a StringView */
static int sv_match(const char *str, StringView sv) {
    size_t len = strlen(str);
    if (len != sv.len) return 0;
    return strncmp(str, sv.data, sv.len) == 0;
}

static void update_status_counts(AnalyzerContext *ctx, int status) {
    if (status >= 200 && status < 300) ctx->status_2xx++;
    else if (status >= 300 && status < 400) ctx->status_3xx++;
    else if (status >= 400 && status < 500) ctx->status_4xx++;
    else if (status >= 500 && status < 600) ctx->status_5xx++;
}

static void aggregate_endpoint(AnalyzerContext *ctx, const LogRecord *record) {
    /* Search for an existing endpoint */
    for (int i = 0; i < ctx->endpoint_count; i++) {
        if (sv_match(ctx->endpoints[i].endpoint, record->endpoint)) {
            ctx->endpoints[i].hits++;
            ctx->endpoints[i].total_bytes += record->response_size;
            if (record->status_code >= 400) ctx->endpoints[i].errors++;
            return; /* Found and updated, return early */
        }
    }
    
    /* Not found, add a new one if there is space */
    if (ctx->endpoint_count < MAX_TRACKED_ITEMS) {
        EndpointStat *new_ep = &ctx->endpoints[ctx->endpoint_count++];
        
        /* Copy data and ensure null termination */
        size_t copy_len = record->endpoint.len < (MAX_ENDPOINT_LEN - 1) ? record->endpoint.len : (MAX_ENDPOINT_LEN - 1);
        memcpy(new_ep->endpoint, record->endpoint.data, copy_len);
        new_ep->endpoint[copy_len] = '\0';
        
        new_ep->hits = 1;
        new_ep->total_bytes = record->response_size;
        if (record->status_code >= 400) new_ep->errors = 1;
    }
}

static void aggregate_ip(AnalyzerContext *ctx, const LogRecord *record) {
    /* Search for an existing IP address */
    for (int i = 0; i < ctx->ip_count; i++) {
        if (sv_match(ctx->ips[i].ip, record->ip)) {
            ctx->ips[i].hits++;
            return; /* Found and updated, return early */
        }
    }
    
    /* Not found, add a new one if there is space */
    if (ctx->ip_count < MAX_TRACKED_ITEMS) {
        IpStat *new_ip = &ctx->ips[ctx->ip_count++];
        
        size_t copy_len = record->ip.len < (MAX_IP_LEN - 1) ? record->ip.len : (MAX_IP_LEN - 1);
        memcpy(new_ip->ip, record->ip.data, copy_len);
        new_ip->ip[copy_len] = '\0';
        
        new_ip->hits = 1;
    }
}

void analyzer_process_record(AnalyzerContext *ctx, const LogRecord *record) {
    if (ctx == NULL || record == NULL) return;

    ctx->total_requests++;
    update_status_counts(ctx, record->status_code);
    aggregate_endpoint(ctx, record);
    aggregate_ip(ctx, record);
}
