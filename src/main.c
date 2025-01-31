#include "LinkedList/test_linkedlist.h"
#include "Stack/test_stack.h"
#include "DoubleLinkedList/test_double_linkedlist.h"
#include "Queue/test_queue.h"
#include "Deque/test_deque.h"
#include "CircularLinkedList/test_circular_list.h"

int main(void) {
    // Old tests
    testLinkedList();
    testStack();

    // New tests
    testDoubleLinkedList();
    testQueue();

    testDeque();

    testCircularList();

    return 0;
}
