#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "analyzer.h"
#include "report.h"

#define MAX_LINE_LEN 4096

int main(int argc, char *argv[]) {
    /* 1. Check command line arguments */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <logfile.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    
    /* 2. Open the file */
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return EXIT_FAILURE;
    }

    /* 3. Allocate context dynamically */
    AnalyzerContext *ctx = malloc(sizeof(AnalyzerContext));
    if (ctx == NULL) {
        fprintf(stderr, "Error: Out of memory for AnalyzerContext.\n");
        fclose(fp);
        return EXIT_FAILURE;
    }
    analyzer_init(ctx);

    /* 4. Process the file line by line */
    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), fp) != NULL) {
        size_t len = strlen(line);
        
        /* Remove newline characters from the end */
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[--len] = '\0';
        }

        if (len == 0) continue; /* Skip empty lines */

        /* Parse and analyze the record */
        LogRecord record;
        if (parse_log_line(line, &record) == 0) {
            analyzer_process_record(ctx, &record);
        }
    }

    /* Check for I/O errors and print the report */
    if (ferror(fp)) {
        fprintf(stderr, "Error: Failed while reading the file.\n");
    } else {
        report_print(ctx);
    }

    free(ctx);
    fclose(fp);

    return EXIT_SUCCESS;
}
