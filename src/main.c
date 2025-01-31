#include <stdio.h>
#include <stdlib.h>
#include "linkedList.h"

// Example print function for integer data
void printInt(const void* data) {
    // Cast the void* back to int* and print
    printf("%d -> ", *(int*)data);
}

int main() {
    Node* head = NULL;

    // Insert some integers
    int a = 10, b = 20, c = 30, d = 40;
    insertAtBeginning(&head, &a, sizeof(int));
    insertAtBeginning(&head, &b, sizeof(int));
    insertAtEnd(&head, &c, sizeof(int));
    insertAtEnd(&head, &d, sizeof(int));

    // Print the list
    printf("List of integers:\n");
    printList(head, printInt);

    // Free the list
    freeList(&head);

    // Check that the list is freed
    printf("List after freeList:\n");
    printList(head, printInt);  // Should print just "NULL"

    return 0;
}

