#include <stddef.h>
#include <malloc.h>

typedef struct Node {
    size_t value;
    struct Node* next;
} Node;

typedef struct LinkedList {
    Node* head;
    size_t len;
} LinkedList;

typedef struct PopResult {
    size_t value;
    bool valid;
} PopResult;

LinkedList lnew() {
    LinkedList node = {nullptr, 0};
    return node;
}

bool lpush(LinkedList* list, size_t value) {
    Node* node = malloc(sizeof(Node));

    if (node == nullptr) {
        return false;
    }

    node->value = value;
    node->next = list->head;
    list->head = node;
    list->len++;
}

PopResult lpop(LinkedList* list) {
    if (list->len == 0) {
        PopResult result = {0, false};
        return result;
    }

    Node* head = list->head;
    size_t value = head->value;
    list->head = head->next;
    free(head);
    list->len--;
    PopResult result = {value, true};
    return result;
}

Node* nthNode(LinkedList* list, size_t index) {
    size_t current = list->len - 1;
    Node* node = list->head;

    // assumes pointers are valid based on length
    while (current > index) {
        node = node->next;
        current--;
    }

    return node;
}

bool linsert(LinkedList* list, size_t index, size_t value) {
    if (index > list->len) {
        return false;
    }

    if (list->len == index) {
        lpush(list, value);
        return true;
    }

    Node* node = nthNode(list, index);
    Node* newNode = malloc(sizeof(Node));

    if (newNode == nullptr) {
        return false;
    }

    newNode->value = value;
    newNode->next = node->next;
    node->next = newNode;
    list->len++;

    return true;
}

size_t* lget(LinkedList* list, size_t index) {
    if (index >= list->len) {
        return nullptr;
    }

    return &nthNode(list, index)->value;
}

size_t* lpeek(LinkedList* list) {
    return lget(list, list->len - 1);
}

size_t llen(LinkedList* list) {
    return list->len;
}

bool lremove(LinkedList* list, size_t index) {
    if (index >= list->len) {
        return false;
    }

    if (index == list->len - 1) {
        return lpop(list).valid;
    }

    Node* node = nthNode(list, index + 1);
    Node* removed = node->next;
    node->next == removed->next;
    free(removed);
    list->len--;

    return true;
}

void ldelete(LinkedList list) {
    size_t current = list.len;
    Node* node = list.head;

    // assumes pointers are valid based on length
    while (current > 0) {
        Node* next = node->next;
        free(node);
        node = next;
        current--;
    }
}
