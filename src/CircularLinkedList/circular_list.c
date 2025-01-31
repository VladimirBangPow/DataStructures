#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "circular_list.h"

static CNode* createCNode(const void* data, size_t data_size) {
    CNode* node = (CNode*)malloc(sizeof(CNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed (CNode).\n");
        exit(EXIT_FAILURE);
    }
    node->data = malloc(data_size);
    if (!node->data) {
        fprintf(stderr, "Memory allocation failed (node->data).\n");
        free(node);
        exit(EXIT_FAILURE);
    }
    memcpy(node->data, data, data_size);

    node->data_size = data_size;
    node->next = NULL;
    return node;
}

void clistInit(CircularList* list) {
    list->tail = NULL;
}

// Insert at front, i.e. after tail->next or creating the first node
void clistInsertFront(CircularList* list, const void* data, size_t data_size) {
    CNode* newNode = createCNode(data, data_size);

    if (list->tail == NULL) {
        // Empty list: newNode points to itself
        list->tail = newNode;
        newNode->next = newNode;
    } else {
        // Insert at head, which is tail->next
        CNode* head = list->tail->next;
        newNode->next = head;
        list->tail->next = newNode;
    }
}

// Insert at back, i.e. effectively new tail
void clistInsertBack(CircularList* list, const void* data, size_t data_size) {
    CNode* newNode = createCNode(data, data_size);

    if (list->tail == NULL) {
        // Empty list
        list->tail = newNode;
        newNode->next = newNode;
    } else {
        // Insert after tail, new tail is newNode
        newNode->next = list->tail->next; // preserve the old head
        list->tail->next = newNode;
        list->tail = newNode;
    }
}

// Remove the front node (tail->next)
int clistRemoveFront(CircularList* list, void* outData) {
    if (list->tail == NULL) {
        return 0; // Empty
    }
    CNode* head = list->tail->next; // front node
    if (head == list->tail) {
        // Only one node in the list
        if (outData) {
            memcpy(outData, head->data, head->data_size);
        }
        free(head->data);
        free(head);
        list->tail = NULL;
        return 1;
    }
    // More than one node
    if (outData) {
        memcpy(outData, head->data, head->data_size);
    }
    list->tail->next = head->next; // skip the old head
    free(head->data);
    free(head);
    return 1;
}

// Remove the back node (tail)
int clistRemoveBack(CircularList* list, void* outData) {
    if (list->tail == NULL) {
        return 0; // Empty
    }
    // If there's only one node
    if (list->tail->next == list->tail) {
        if (outData) {
            memcpy(outData, list->tail->data, list->tail->data_size);
        }
        free(list->tail->data);
        free(list->tail);
        list->tail = NULL;
        return 1;
    }
    // More than one node: find the node before tail
    CNode* cur = list->tail->next;  // Start at head
    while (cur->next != list->tail) {
        cur = cur->next;
    }
    // 'cur' is now the node before tail
    if (outData) {
        memcpy(outData, list->tail->data, list->tail->data_size);
    }
    cur->next = list->tail->next; // skip the old tail
    free(list->tail->data);
    free(list->tail);
    list->tail = cur; // new tail
    return 1;
}

void clistPrint(const CircularList* list, CPrintFunc printFunc) {
    if (list->tail == NULL) {
        // Empty
        printf("NULL\n");
        return;
    }
    CNode* head = list->tail->next;
    CNode* temp = head;
    do {
        printFunc(temp->data);
        temp = temp->next;
    } while (temp != head);
    printf("NULL\n");
}

void clistFree(CircularList* list) {
    // Keep removing front until empty
    while (clistRemoveFront(list, NULL)) {
        // no-op
    }
}
