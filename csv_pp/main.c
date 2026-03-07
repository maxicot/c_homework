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

void printBorder(char ch, size_t maxCols, const size_t* colWidths)
{
    putchar('+');

    for (size_t i = 0; i < maxCols; i++) {
        for (size_t w = 0; w < colWidths[i] + 2; w++) {
            putchar(ch);
        }

        putchar('+');
    }

    putchar('\n');
}

// Usage example:
// ```
// ./main < foo.csv
// ```
int main()
{
    char** lines = nullptr;
    size_t lineCount = 0;
    size_t maxCols = 0;
    char* buf = nullptr;
    size_t bufSize = 0;

    while (getline(&buf, &bufSize, stdin) != -1) {
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
            return ENOMEM;
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
        return 0;
    }

    size_t* colWidths = calloc(maxCols, sizeof(size_t));

    if (colWidths == nullptr) {
        return ENOMEM;
    }

    for (size_t i = 0; i < lineCount; i++) {
        size_t cnt = 0;
        char** fields = parseLine(lines[i], &cnt);

        if (fields == nullptr) {
            return ENOMEM;
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

    printBorder('=', maxCols, colWidths);

    for (size_t i = 0; i < lineCount; i++) {
        size_t cnt = 0;
        char** fields = parseLine(lines[i], &cnt);

        if (fields == nullptr) {
            return ENOMEM;
        }

        putchar('|');

        for (size_t j = 0; j < maxCols; j++) {
            const char* val = (j < cnt) ? fields[j] : "";
            putchar(' ');

            if (i == 0) {
                printf("%-*s", (int)colWidths[j], val);
            } else {
                if (isNumeric(val)) {
                    printf("%*s", (int)colWidths[j], val);
                } else {
                    printf("%-*s", (int)colWidths[j], val);
                }
            }

            putchar(' ');
            putchar('|');
        }

        putchar('\n');

        for (size_t j = 0; j < cnt; j++) {
            free(fields[j]);
        }

        free(fields);
        printBorder(i == 0 ? '=' : '-', maxCols, colWidths);
    }

    for (size_t i = 0; i < lineCount; i++) {
        free(lines[i]);
    }

    free(lines);
    free(colWidths);

    return 0;
}
