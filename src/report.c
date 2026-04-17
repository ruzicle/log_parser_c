#include "report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOP_ENDPOINTS_LIMIT 5
#define TOP_IPS_LIMIT 3
#define SCRAPER_REQ_THRESHOLD 100
#define SCRAPER_RATIO 0.1

/* Comparators for qsort, descending order */
static int cmp_endpoints(const void *a, const void *b) {
    return ((const EndpointStat *)b)->hits - ((const EndpointStat *)a)->hits;
}

static int cmp_ips(const void *a, const void *b) {
    return ((const IpStat *)b)->hits - ((const IpStat *)a)->hits;
}

static void print_top_endpoints(const EndpointStat *sorted_eps, int count) {
    printf("Top %d endpoints by request volume:\n", TOP_ENDPOINTS_LIMIT);
    int ep_limit = (count < TOP_ENDPOINTS_LIMIT) ? count : TOP_ENDPOINTS_LIMIT;
    
    for (int i = 0; i < ep_limit; i++) {
        double err_rate = (sorted_eps[i].errors * 100.0) / sorted_eps[i].hits;
        long avg_bytes = sorted_eps[i].total_bytes / sorted_eps[i].hits;
        
        printf("  %-20s -> %5d hits | avg %6ld B | %4.1f%% error rate\n",
               sorted_eps[i].endpoint,  
               sorted_eps[i].hits,     
               avg_bytes,         
               err_rate);         
    }
}

static void print_status_distribution(const AnalyzerContext *ctx) {
    printf("\nStatus code distribution:\n");
    printf("  2xx: %4.1f%%\n", (ctx->status_2xx * 100.0) / ctx->total_requests);
    printf("  3xx: %4.1f%%\n", (ctx->status_3xx * 100.0) / ctx->total_requests);
    printf("  4xx: %4.1f%%\n", (ctx->status_4xx * 100.0) / ctx->total_requests);
    printf("  5xx: %4.1f%%\n", (ctx->status_5xx * 100.0) / ctx->total_requests);
}

static void print_top_ips(const AnalyzerContext *ctx, const IpStat *sorted_ips) {
    printf("\nTop %d IPs by request count:\n", TOP_IPS_LIMIT);
    int ip_limit = (ctx->ip_count < TOP_IPS_LIMIT) ? ctx->ip_count : TOP_IPS_LIMIT;
    
    for (int i = 0; i < ip_limit; i++) {
        printf("  %-16s -> %5d requests", sorted_ips[i].ip, sorted_ips[i].hits);
               
        /* Identify potential scrapers based on request volume */
        if (ctx->total_requests > SCRAPER_REQ_THRESHOLD && 
           (sorted_ips[i].hits > ctx->total_requests * SCRAPER_RATIO)) {
            printf("  [POSSIBLE SCRAPER]");
        }
        printf("\n");
    }
    printf("\n");
}

/* Main reporting function */
void report_print(const AnalyzerContext *ctx) {
    if (ctx == NULL || ctx->total_requests == 0) {
        printf("No data to report.\n");
        return;
    }

    EndpointStat *sorted_eps = malloc(ctx->endpoint_count * sizeof(EndpointStat));
    IpStat *sorted_ips = malloc(ctx->ip_count * sizeof(IpStat));
    
    if (sorted_eps == NULL || sorted_ips == NULL) {
        printf("Error: Memory allocation failed.\n");
        free(sorted_eps);
        free(sorted_ips);
        return;
    }
    
    /* Copy data to avoid modifying the original context during sorting */
    memcpy(sorted_eps, ctx->endpoints, ctx->endpoint_count * sizeof(EndpointStat));
    memcpy(sorted_ips, ctx->ips, ctx->ip_count * sizeof(IpStat));

    qsort(sorted_eps, ctx->endpoint_count, sizeof(EndpointStat), cmp_endpoints);
    qsort(sorted_ips, ctx->ip_count, sizeof(IpStat), cmp_ips);

    printf("\n=== Traffic Report ===\n\n");
    print_top_endpoints(sorted_eps, ctx->endpoint_count);
    print_status_distribution(ctx);
    print_top_ips(ctx, sorted_ips);

    free(sorted_eps);
    free(sorted_ips);
}
