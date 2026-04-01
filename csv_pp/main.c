#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool isNumeric(const char* s)
{
    char* end = nullptr;
    strtod(s, &end);

    return *s && *end == '\0';
}

char** parseLine(const char* line, size_t* fieldCount)
{
    size_t cap = 8;
    size_t cnt = 0;
    char** fields = malloc(cap * sizeof(char*));

    if (fields == nullptr) {
        return nullptr;
    }

    const char* start = line;
    const char* p = line;

    while (*p) {
        if (*p == ',') {
            size_t len = p - start;
            fields[cnt] = malloc(len + 1);

            if (fields[cnt] == nullptr) {
                return nullptr;
            }

            strncpy(fields[cnt], start, len);
            fields[cnt][len] = '\0';
            start = p + 1;
            cnt++;

            if (cnt == cap) {
                cap *= 2;
                char** newFields = realloc(fields, cap * sizeof(char*));

                if (newFields == nullptr) {
                    return nullptr;
                }

                fields = newFields;
            }
        }

        p++;
    }

    size_t len = p - start;
    fields[cnt] = malloc(len + 1);

    if (fields[cnt] == nullptr) {
        return nullptr;
    }

    strncpy(fields[cnt], start, len);
    fields[cnt][len] = '\0';
    cnt++;
    *fieldCount = cnt;

    return fields;
}

void readCsv(FILE* file, char*** linesOut, size_t* lineCountOut, size_t* maxColsOut)
{
    char** lines = nullptr;
    size_t lineCount = 0;
    size_t maxCols = 0;
    char* buf = nullptr;
    size_t bufSize = 0;

    while (getline(&buf, &bufSize, file) != -1) {
        size_t len = strlen(buf);

        if (len > 0 && buf[len - 1] == '\n') {
            buf[--len] = '\0';
        }

        if (len > 0 && buf[len - 1] == '\r') {
            buf[--len] = '\0';
        }

        char** newLines = realloc(lines, (lineCount + 1) * sizeof(char*));

        if (newLines == nullptr) {
            free(lines);
            exit(ENOMEM);
        }

        lines = newLines;

        lines[lineCount] = strdup(buf);

        size_t cnt = 1;

        for (const char* p = buf; *p; p++) {
            if (*p == ',') {
                cnt++;
            }
        }

        if (cnt > maxCols) {
            maxCols = cnt;
        }

        lineCount++;
    }

    free(buf);

    if (lineCount == 0) {
        fprintf(stderr, "Empty file");
        exit(ENODATA);
    }

    *linesOut = lines;
    *lineCountOut = lineCount;
    *maxColsOut = maxCols;
}

size_t* parseLines(char** lines, size_t lineCount, size_t maxCols)
{
    size_t* colWidths = calloc(maxCols, sizeof(size_t));

    if (colWidths == nullptr) {
        exit(ENOMEM);
    }

    for (size_t i = 0; i < lineCount; i++) {
        size_t cnt = 0;
        char** fields = parseLine(lines[i], &cnt);

        if (fields == nullptr) {
            exit(ENOMEM);
        }

        for (size_t j = 0; j < cnt; j++) {
            size_t w = strlen(fields[j]);

            if (w > colWidths[j]) {
                colWidths[j] = w;
            }
        }

        for (size_t j = 0; j < cnt; j++) {
            free(fields[j]);
        }

        free(fields);
    }

    return colWidths;
}

void fprintBorder(char ch, size_t maxCols, const size_t* colWidths, FILE* file)
{
    fputc('+', file);

    for (size_t i = 0; i < maxCols; i++) {
        for (size_t w = 0; w < colWidths[i] + 2; w++) {
            fputc(ch, file);
        }

        fputc('+', file);
    }

    fputc('\n', file);
}

void fprintCsvPretty(FILE* file, char** lines, size_t lineCount, size_t maxCols, const size_t* colWidths)
{
    fprintBorder('=', maxCols, colWidths, file);

    for (size_t i = 0; i < lineCount; i++) {
        size_t cnt = 0;
        char** fields = parseLine(lines[i], &cnt);

        if (fields == nullptr) {
            exit(ENOMEM);
        }

        fputc('|', file);

        for (size_t j = 0; j < maxCols; j++) {
            const char* val = (j < cnt) ? fields[j] : "";
            fputc(' ', file);

            if (i == 0) {
                fprintf(file, "%-*s", (int)colWidths[j], val);
            } else {
                if (isNumeric(val)) {
                    fprintf(file, "%*s", (int)colWidths[j], val);
                } else {
                    fprintf(file, "%-*s", (int)colWidths[j], val);
                }
            }

            fputc(' ', file);
            fputc('|', file);
        }

        fputc('\n', file);

        for (size_t j = 0; j < cnt; j++) {
            free(fields[j]);
        }

        free(fields);
        fprintBorder(i == 0 ? '=' : '-', maxCols, colWidths, file);
    }
}

// Usage example:
// ```
// ./csv_pp input.csv output.txt
// ```
int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: ./csv_pp input.csv output.txt");
        return EINVAL;
    }

    char** lines = nullptr;
    size_t lineCount = 0;
    size_t maxCols = 0;
    FILE* input = fopen(argv[1], "r");

    if (input == nullptr) {
        return errno;
    }

    readCsv(input, &lines, &lineCount, &maxCols);
    size_t* colWidths = parseLines(lines, lineCount, maxCols);
    FILE* output = fopen(argv[2], "w");

    if (output == nullptr) {
        fprintf(stderr, "Cannot open specified output file. Printing to stdout instead.\n");
        output = stdout;
    }

    fprintCsvPretty(output, lines, lineCount, maxCols, colWidths);

    for (size_t i = 0; i < lineCount; i++) {
        free(lines[i]);
    }

    free(lines);
    free(colWidths);

    return 0;
}
