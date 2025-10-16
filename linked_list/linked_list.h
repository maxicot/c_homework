#pragma once
#include <stddef.h>
#include "linked_list.c"

// Singly linked list of `size_t` elements.
// Thus, it can store any integers with size less than or equal to that of `size_t`.
// WARNING: manual access to the fields is likely to result in UB
typedef struct LinkedList LinkedList;

// A result type for the `pop` operation with two fields:
// `value` of `size_t` and `valid` of `bool`
typedef struct PopResult PopResult;

// Return an empty list
LinkedList lnew();

// Push a `size_t` to the top of the list.
// Return whether successful
bool lpush(LinkedList* list, size_t value);

// Remove and return a `size_t` from the top of the list
PopResult lpop(LinkedList* list);

// Insert a `size_t` to the list at a given index.
// Return whether the insertion was successful
bool linsert(LinkedList* list, size_t index, size_t value);

// Return a mutable pointer to the size_t at the given index.
// Returns nullptr if the index is out of bounds
size_t* lget(LinkedList* list, size_t index);

// Return a mutable pointer to the top of the list.
// Returns nullptr if the list is empty
size_t* lpeek(LinkedList* list);

// Return the list's length
size_t llen(LinkedList* list);

// Remove the element at a given index from the list.
// Return whether successful
bool lremove(LinkedList* list, size_t index);

// Free the list's nodes
void ldelete(LinkedList list);
