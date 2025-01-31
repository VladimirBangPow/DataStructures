#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "deque.h"


void dequePrint(const Deque* dq, void (*printFunc)(const void*)) {
    if (dequeIsEmpty(dq)) {
        // If the deque is empty, just print NULL
        printf("NULL\n");
        return;
    }

    // Start at the front
    DequeNode* current = dq->front;
    while (1) {
        // Use the user-provided print function
        printFunc(current->data);

        // If we've reached the 'back' node, we're done
        if (current == dq->back) {
            break;
        }
        current = current->next;
    }
    printf("NULL\n");
}

static DequeNode* createDequeNode(const void* data, size_t data_size) {
    DequeNode* node = (DequeNode*)malloc(sizeof(DequeNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed (DequeNode).\n");
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
    node->prev = node->next = NULL;
    return node;
}

void dequeInit(Deque* dq) {
    dq->front = dq->back = NULL;
}

void dequePushFront(Deque* dq, const void* data, size_t data_size) {
    DequeNode* newNode = createDequeNode(data, data_size);
    if (dq->front == NULL) {
        // Empty deque: front and back both become newNode
        dq->front = dq->back = newNode;
    } else {
        newNode->next = dq->front;
        dq->front->prev = newNode;
        dq->front = newNode;
    }
}

void dequePushBack(Deque* dq, const void* data, size_t data_size) {
    DequeNode* newNode = createDequeNode(data, data_size);
    if (dq->back == NULL) {
        // Empty deque: front and back both become newNode
        dq->front = dq->back = newNode;
    } else {
        newNode->prev = dq->back;
        dq->back->next = newNode;
        dq->back = newNode;
    }
}

int dequePopFront(Deque* dq, void* outData) {
    if (dq->front == NULL) {
        // Deque is empty
        return 0;
    }
    DequeNode* temp = dq->front;
    if (outData) {
        memcpy(outData, temp->data, temp->data_size);
    }

    dq->front = temp->next;
    if (dq->front) {
        dq->front->prev = NULL;
    } else {
        // Deque becomes empty
        dq->back = NULL;
    }

    free(temp->data);
    free(temp);
    return 1;
}

int dequePopBack(Deque* dq, void* outData) {
    if (dq->back == NULL) {
        // Deque is empty
        return 0;
    }
    DequeNode* temp = dq->back;
    if (outData) {
        memcpy(outData, temp->data, temp->data_size);
    }

    dq->back = temp->prev;
    if (dq->back) {
        dq->back->next = NULL;
    } else {
        // Deque becomes empty
        dq->front = NULL;
    }

    free(temp->data);
    free(temp);
    return 1;
}

int dequeIsEmpty(const Deque* dq) {
    return (dq->front == NULL);
}

void dequeClear(Deque* dq) {
    while (!dequeIsEmpty(dq)) {
        dequePopFront(dq, NULL);
    }
}
