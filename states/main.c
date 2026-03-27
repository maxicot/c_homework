#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t to;
    int len;
} Edge;

typedef struct {
    Edge* items;
    size_t count;
    size_t capacity;
} EdgeVector;

void edgeVecInit(EdgeVector* vec)
{
    vec->items = nullptr;
    vec->count = 0;
    vec->capacity = 0;
}

void edgeVecPush(EdgeVector* vec, Edge e)
{
    if (vec->count >= vec->capacity) {
        vec->capacity = (vec->capacity == 0) ? 4 : vec->capacity * 2;
        Edge* newItems = realloc(vec->items, vec->capacity * sizeof(Edge));

        if (newItems == nullptr) {
            fprintf(stderr, "realloc edge vector failed: %s\n", strerror(errno));
            exit(ENOMEM);
        }

        vec->items = newItems;
    }

    vec->items[vec->count++] = e;
}

void edgeVecFree(EdgeVector* vec)
{
    free(vec->items);
    vec->items = nullptr;
    vec->count = 0;
    vec->capacity = 0;
}

typedef struct {
    size_t* items;
    size_t count;
    size_t capacity;
} CityVector;

void cityVecInit(CityVector* vec)
{
    vec->items = nullptr;
    vec->count = 0;
    vec->capacity = 0;
}

void cityVecPush(CityVector* vec, size_t val)
{
    if (vec->count >= vec->capacity) {
        vec->capacity = (vec->capacity == 0) ? 4 : vec->capacity * 2;
        size_t* newItems = realloc(vec->items, vec->capacity * sizeof(size_t));

        if (newItems == nullptr) {
            fprintf(stderr, "realloc city vector failed: %s\n", strerror(errno));
            exit(ENOMEM);
        }

        vec->items = newItems;
    }

    vec->items[vec->count++] = val;
}

void cityVecFree(CityVector* vec)
{
    free(vec->items);
    vec->items = nullptr;
    vec->count = 0;
    vec->capacity = 0;
}

int compareSizeT(const void* a, const void* b)
{
    size_t ia = *(const size_t*)a;
    size_t ib = *(const size_t*)b;

    return (ia > ib) - (ia < ib);
}

typedef struct HeapNode {
    size_t city;
    int dist;
} HeapNode;

typedef struct {
    HeapNode* items;
    size_t count;
    size_t capacity;
} Heap;

void heapInit(Heap* h)
{
    h->items = nullptr;
    h->count = 0;
    h->capacity = 0;
}

void heapPush(Heap* h, size_t city, int dist)
{
    if (h->count >= h->capacity) {
        h->capacity = (h->capacity == 0) ? 4 : h->capacity * 2;
        void* newItems = realloc(h->items, h->capacity * sizeof(HeapNode));

        if (newItems == nullptr) {
            fprintf(stderr, "realloc heap failed: %s\n", strerror(errno));
            exit(ENOMEM);
        }

        h->items = newItems;
    }

    size_t idx = h->count++;
    h->items[idx].city = city;
    h->items[idx].dist = dist;

    while (idx > 0) {
        size_t parent = (idx - 1) / 2;

        if (h->items[parent].dist <= h->items[idx].dist) {
            break;
        }

        HeapNode tmp = h->items[parent];
        h->items[parent] = h->items[idx];
        h->items[idx] = tmp;
        idx = parent;
    }
}

void heapPop(Heap* h)
{
    if (h->count == 0) {
        return;
    }

    h->count--;

    if (h->count > 0) {
        h->items[0] = h->items[h->count];
        size_t idx = 0;

        for (;;) {
            size_t left = 2 * idx + 1;
            size_t right = 2 * idx + 2;
            size_t smallest = idx;

            if (left < h->count && h->items[left].dist < h->items[smallest].dist) {
                smallest = left;
            }

            if (right < h->count && h->items[right].dist < h->items[smallest].dist) {
                smallest = right;
            }

            if (smallest == idx) {
                break;
            }

            HeapNode tmp = h->items[idx];
            h->items[idx] = h->items[smallest];
            h->items[smallest] = tmp;
            idx = smallest;
        }
    }
}

bool heapEmpty(const Heap* h)
{
    return h->count == 0;
}

void heapFree(Heap* h)
{
    free(h->items);
    h->items = nullptr;
    h->count = 0;
    h->capacity = 0;
}

// Usage: `./states file.txt1`
int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);

        return EINVAL;
    }

    FILE* inputFile = fopen(argv[1], "r");

    if (inputFile == nullptr) {
        fprintf(stderr, "Error opening file '%s': %s (errno %d)\n", argv[1], strerror(errno), errno);

        return errno;
    }

    int nInt = 0, mInt = 0;

    if (fscanf(inputFile, "%d %d", &nInt, &mInt) != 2) {
        fprintf(stderr, "Error reading n and m\n");
        fclose(inputFile);

        return errno;
    }

    size_t n = (size_t)nInt;
    size_t m = (size_t)mInt;
    EdgeVector* adj = malloc(n * sizeof(EdgeVector));

    if (adj == nullptr) {
        fprintf(stderr, "malloc adj failed: %s\n", strerror(errno));
        fclose(inputFile);

        return ENOMEM;
    }

    for (size_t i = 0; i < n; i++) {
        edgeVecInit(&adj[i]);
    }

    for (size_t i = 0; i < m; i++) {
        int uInt = 0, vInt = 0, len = 0;

        if (fscanf(inputFile, "%d %d %d", &uInt, &vInt, &len) != 3) {
            fprintf(stderr, "Error reading edge\n");
            fclose(inputFile);

            return errno;
        }

        size_t u = (size_t)(uInt - 1);
        size_t v = (size_t)(vInt - 1);

        edgeVecPush(&adj[u], (Edge) { v, len });
        edgeVecPush(&adj[v], (Edge) { u, len });
    }

    int kInt = 0;

    if (fscanf(inputFile, "%d", &kInt) != 1) {
        fprintf(stderr, "Error reading k\n");
        fclose(inputFile);

        return errno;
    }

    size_t k = (size_t)kInt;
    size_t* capitals = malloc(k * sizeof(size_t));

    if (capitals == nullptr) {
        fprintf(stderr, "malloc capitals failed: %s\n", strerror(errno));
        fclose(inputFile);

        return ENOMEM;
    }

    for (size_t i = 0; i < k; i++) {
        int capInt = 0;

        if (fscanf(inputFile, "%d", &capInt) != 1) {
            fprintf(stderr, "Error reading capital\n");
            fclose(inputFile);

            return errno;
        }

        capitals[i] = (size_t)(capInt - 1);
    }

    fclose(inputFile);
    int* assigned = calloc(n, sizeof(int));

    if (assigned == nullptr) {
        fprintf(stderr, "calloc assigned failed: %s\n", strerror(errno));

        return ENOMEM;
    }

    CityVector* stateCities = malloc(k * sizeof(CityVector));
    Heap* heaps = malloc(k * sizeof(Heap));

    if (stateCities == nullptr || heaps == nullptr) {
        fprintf(stderr, "malloc stateCities/heaps failed: %s\n", strerror(errno));

        return ENOMEM;
    }

    for (size_t i = 0; i < k; i++) {
        cityVecInit(&stateCities[i]);
        heapInit(&heaps[i]);
    }

    size_t totalAssigned = 0;

    for (size_t i = 0; i < k; i++) {
        size_t cap = capitals[i];
        assigned[cap] = (int)(i + 1);
        cityVecPush(&stateCities[i], cap);
        totalAssigned++;

        for (size_t j = 0; j < adj[cap].count; j++) {
            Edge e = adj[cap].items[j];

            if (assigned[e.to] == 0) {
                heapPush(&heaps[i], e.to, e.len);
            }
        }
    }

    while (totalAssigned < n) {
        bool anyAssigned = false;

        for (size_t i = 0; i < k; i++) {
            if (heapEmpty(&heaps[i])) {
                continue;
            }

            while (!heapEmpty(&heaps[i])) {
                size_t city = heaps[i].items[0].city;
                heapPop(&heaps[i]);

                if (assigned[city] == 0) {
                    assigned[city] = (int)(i + 1);
                    cityVecPush(&stateCities[i], city);
                    totalAssigned++;
                    anyAssigned = true;

                    for (size_t j = 0; j < adj[city].count; j++) {
                        Edge e = adj[city].items[j];

                        if (assigned[e.to] == 0) {
                            heapPush(&heaps[i], e.to, e.len);
                        }
                    }

                    break;
                }
            }
        }

        if (!anyAssigned) {
            break;
        }
    }

    for (size_t i = 0; i < k; i++) {
        qsort(stateCities[i].items, stateCities[i].count,
            sizeof(size_t), compareSizeT);

        printf("State %zu:", i + 1);

        for (size_t j = 0; j < stateCities[i].count; j++) {
            printf(" %zu", stateCities[i].items[j] + 1);
        }

        printf("\n");
    }

    for (size_t i = 0; i < n; i++) {
        edgeVecFree(&adj[i]);
    }

    free(adj);

    for (size_t i = 0; i < k; i++) {
        cityVecFree(&stateCities[i]);
        heapFree(&heaps[i]);
    }

    free(stateCities);
    free(heaps);
    free(assigned);
    free(capitals);

    return 0;
}
