#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct AVL32Node {
    void* value;
    struct AVL32Node* left;
    struct AVL32Node* right;
    uint32_t key;
    int8_t diff;
} AVL32Node;

typedef struct AVL32Map {
    AVL32Node* root;
    size_t size;
} AVL32Map;

AVL32Map avl32MapNew()
{
    return (AVL32Map) {
        .root = nullptr,
        .size = 0
    };
}

int8_t avl32NodeDiff(AVL32Node* node)
{
    if (node == nullptr) {
        return 0;
    }

    return node->diff;
}

AVL32Node* avl32NodeRotateLeft(AVL32Node* a)
{
    AVL32Node* b = a->right;
    AVL32Node* c = b->left;
    b->left = a;
    a->right = c;
    a->diff = avl32NodeDiff(a->right) - avl32NodeDiff(a->left);
    b->diff = avl32NodeDiff(b->right) - avl32NodeDiff(b->left);

    return b;
}

AVL32Node* avl32NodeRotateRight(AVL32Node* b)
{
    AVL32Node* a = b->left;
    AVL32Node* c = a->right;
    a->right = b;
    b->left = c;
    a->diff = avl32NodeDiff(a->right) - avl32NodeDiff(a->left);
    b->diff = avl32NodeDiff(b->right) - avl32NodeDiff(b->left);

    return a;
}

AVL32Node* avl32NodeBalance(AVL32Node* node)
{
    if (node == nullptr) {
        return nullptr;
    }

    if (avl32NodeDiff(node) > 1) {
        if (avl32NodeDiff(node->right) >= 0) {
            return avl32NodeRotateLeft(node);
        }

        node->left = avl32NodeRotateLeft(node->left);
        return avl32NodeRotateRight(node);
    }

    if (avl32NodeDiff(node) < -1) {
        if (avl32NodeDiff(node->left) <= 0) {
            return avl32NodeRotateRight(node);
        }

        node->right = avl32NodeRotateRight(node->right);
        return avl32NodeRotateLeft(node);
    }

    return node;
}

AVL32Node* avl32MapInsertNode(AVL32Node* node, uint32_t key, void* value, void** status)
{
    if (node == nullptr) {
        AVL32Node* new = calloc(1, sizeof(AVL32Node));

        if (new == nullptr) {
            *status = nullptr;
            return nullptr;
        }

        new->key = key;
        new->value = value;
        *status = nullptr;

        return new;
    }

    if (key < node->key) {
        node->left = avl32MapInsertNode(node->left, key, value, status);
        node->diff--;
    } else if (key > node->key) {
        node->right = avl32MapInsertNode(node->right, key, value, status);
        node->diff++;
    } else {
        *status = node->value;
        node->value = value;

        return node;
    }

    return avl32NodeBalance(node);
}

void* avl32MapInsert(AVL32Map* map, uint32_t key, void* value)
{
    void* status = nullptr;
    AVL32Node* new = avl32MapInsertNode(map->root, key, value, &status);

    if (new != nullptr) {
        map->root = new;
    }

    if (status == nullptr) {
        map->size++;
    }

    return status;
}

AVL32Node* avl32LeftmostChild(AVL32Node* node)
{
    if (node == nullptr) {
        return nullptr;
    }

    AVL32Node* current = node;

    while (current->left != nullptr) {
        current = current->left;
    }

    return current;
}

AVL32Node* avl32NodeDelete(AVL32Node* node, uint32_t key, void** value)
{
    if (node == nullptr) {
        *value = nullptr;
        return nullptr;
    }

    if (key < node->key) {
        node->left = avl32NodeDelete(node->left, key, value);
    } else if (key > node->key) {
        node->right = avl32NodeDelete(node->right, key, value);
    } else {
        *value = node->value;

        if (node->left == nullptr || node->right == nullptr) {
            AVL32Node* temp = (node->left != nullptr) ? node->left : node->right;
            free(node);
            return temp;
        } else {
            AVL32Node* temp = avl32LeftmostChild(node->right);
            node->key = temp->key;
            node->value = temp->value;
            node->right = avl32NodeDelete(node->right, temp->key, value);
        }
    }

    return avl32NodeBalance(node);
}

void* avl32MapDelete(AVL32Map* map, uint32_t key)
{
    void* value = nullptr;
    AVL32Node* new = avl32NodeDelete(map->root, key, &value);
    map->root = new;

    if (value != nullptr) {
        map->size--;
    }

    return value;
}

void* avl32MapFind(const AVL32Map* map, uint32_t key)
{
    AVL32Node* current = map->root;

    while (current != nullptr) {
        if (key < current->key) {
            current = current->left;
        } else if (key > current->key) {
            current = current->right;
        } else {
            return current->value;
        }
    }

    return nullptr;
}

typedef void (*AVL32IterationFunction)(uint32_t key, void* value, void* parameters);

void avl32MapNodeInorder(AVL32Node* node, AVL32IterationFunction f, void* parameters)
{
    if (node == nullptr) {
        return;
    }

    avl32MapNodeInorder(node->left, f, parameters);
    f(node->key, node->value, parameters);
    avl32MapNodeInorder(node->right, f, parameters);
}

void avl32MapIterate(AVL32Map* map, AVL32IterationFunction f, void* parameters)
{
    avl32MapNodeInorder(map->root, f, parameters);
}

void trimNewline(char* s)
{
    size_t len = strlen(s);

    if (len > 0 && s[len - 1] == '\n') {
        s[len - 1] = '\0';
    }
}

uint32_t packIata(const char code[3])
{
    return ((uint32_t)code[0] << 16) | ((uint32_t)code[1] << 8) | ((uint32_t)code[2]);
}

void unpackIata(uint32_t key, char out[4])
{
    out[0] = (char)(key >> 16);
    out[1] = (char)(key >> 8);
    out[2] = (char)(key);
    out[3] = '\0';
}

void writer(uint32_t key, void* value, void* parameters)
{
    char code[4] = { 0 };
    unpackIata(key, code);
    fprintf((FILE*)parameters, "%s:%s\n", code, (char*)value);
}

void load(AVL32Map* map)
{
    FILE* file = fopen("airports.txt", "r");

    if (file == nullptr) {
        fprintf(stderr, "Couldn't open airports.txt\n");
        return;
    }

    char buf[1024] = { 0 };

    while (fgets(buf, sizeof(buf), file) != nullptr) {
        trimNewline(buf);

        if (buf[0] == '\0') {
            continue;
        }

        char* colon = strchr(buf, ':');

        if (colon == nullptr) {
            fprintf(stderr, "Skipping wrong entry: `%s`\n", buf);
            continue;
        }

        *colon = '\0';
        char* code = buf;
        char* name = colon + 1;

        if (strlen(code) != 3) {
            fprintf(stderr, "Skipping invalid IATA code: `%s`\n", code);
            continue;
        }

        if (*name == '\0') {
            fprintf(stderr, "Skipping empty name for code `%s`\n", code);
        }

        uint32_t key = packIata(code);
        char* nameAlloc = strdup(name);

        if (nameAlloc == nullptr) {
            fprintf(stderr, "Failed to allocate name `%s`\n", name);
            continue;
        }

        char* value = avl32MapInsert(map, key, nameAlloc);

        if (value != nullptr) {
            printf("Entry %s:%s was overwritten\n", code, value);
            free(value);
        }
    }

    fclose(file);
}

int main()
{
    printf("AeroSoft airport database REPL\n");
    printf("Commands: find <IATA code>, add <IATA code>:<airport name>, delete <IATA code>, save, quit\n");

    AVL32Map map = avl32MapNew();
    load(&map);
    char buf[1024] = { 0 };

    for (;;) {
        printf("> ");

        if (fgets(buf, sizeof(buf), stdin) == nullptr) {
            break;
        }

        trimNewline(buf);

        if (buf[0] == '\0') {
            continue;
        }

        char* cmd = strtok(buf, " ");

        if (cmd == nullptr) {
            continue;
        }

        if (strcmp(cmd, "quit") == 0) {
            break;
        } else if (strcmp(cmd, "find") == 0) {
            char* code = strtok(nullptr, " ");

            if (code == nullptr) {
                fprintf(stderr, "Usage: find <code>\n");
                continue;
            }

            if (strlen(code) != 3) {
                fprintf(stderr, "Invalid IATA code\n");
                continue;
            }

            uint32_t key = packIata(code);
            const char* name = avl32MapFind(&map, key);

            if (name == nullptr) {
                printf("Not found\n");
            } else {
                printf("%s\n", name);
            }
        } else if (strcmp(cmd, "add") == 0) {
            char* args = buf + strlen(cmd) + 1;

            while (*args == ' ') {
                args++;
            }

            if (*args == '\0') {
                fprintf(stderr, "Usage: add <code>:<name>\n");
                continue;
            }

            char* colon = strchr(args, ':');

            if (colon == nullptr) {
                fprintf(stderr, "Missing colon. Usage: add <code>:<name>\n");
                continue;
            }

            *colon = '\0';
            char* code = args;
            char* name = colon + 1;

            if (strlen(code) != 3) {
                fprintf(stderr, "Invalid IATA code\n");
                continue;
            }

            if (*name == '\0') {
                fprintf(stderr, "Name cannot be empty\n");
                continue;
            }

            uint32_t key = packIata(code);
            char* nameAlloc = strdup(name);

            if (nameAlloc == nullptr) {
                fprintf(stderr, "ENOMEM: %d\n", ENOMEM);
            }

            char* result = avl32MapInsert(&map, key, nameAlloc);

            if (result == nullptr) {
                printf("Added\n");
            } else {
                printf("Replaced the value '%s'\n", result);
                free(result);
            }
        } else if (strcmp(cmd, "delete") == 0) {
            char* code = strtok(nullptr, " ");

            if (code == nullptr) {
                fprintf(stderr, "Usage: delete <code>\n");
            }

            if (strlen(code) != 3) {
                fprintf(stderr, "Invalid IATA code\n");
            }

            uint32_t key = packIata(code);
            char* name = avl32MapDelete(&map, key);

            if (name == nullptr) {
                printf("Not found\n");
            } else {
                free(name);
                printf("Deleted\n");
            }
        } else if (strcmp(cmd, "save") == 0) {
            FILE* file = fopen("airports.txt", "w");

            if (file == nullptr) {
                fprintf(stderr, "Could not open airports.txt fo writing\n");
                continue;
            }

            avl32MapIterate(&map, writer, file);
            fclose(file);
        } else {
            fprintf(stderr, "Unknown command. Use find, add, delete, save or quit\n");
        }
    }

    return 0;
}
