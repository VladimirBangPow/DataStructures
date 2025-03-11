#ifndef DOUBLE_LINKEDLIST_H
#define DOUBLE_LINKEDLIST_H

#include <stddef.h> // For size_t

// Doubly linked list node
typedef struct DNode {
    void* data;
    size_t data_size;
    struct DNode* prev;
    struct DNode* next;
} DNode;

// Doubly linked list structure
typedef struct {
    DNode* head;
    DNode* tail;
} DoubleLinkedList;

// Function prototypes
void dllInit(DoubleLinkedList* list);
void dllInsertFront(DoubleLinkedList* list, const void* data, size_t data_size);
void dllInsertBack(DoubleLinkedList* list, const void* data, size_t data_size);
int dllRemoveFront(DoubleLinkedList* list, void* outData);
int dllRemoveBack(DoubleLinkedList* list, void* outData);
void dllPrint(const DoubleLinkedList* list, void (*printFunc)(const void*));
void dllFree(DoubleLinkedList* list);

#endif // DOUBLE_LINKEDLIST_H
