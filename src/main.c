#include "LinkedList/test_linkedlist.h"
#include "Stack/test_stack.h"
#include "DoubleLinkedList/test_double_linkedlist.h"
#include "Queue/test_queue.h"

int main(void) {
    // Old tests
    testLinkedList();
    testStack();

    // New tests
    testDoubleLinkedList();
    testQueue();

    return 0;
}
