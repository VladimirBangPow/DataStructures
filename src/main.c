#include "LinkedList/test_linkedlist.h"
#include "Stack/test_stack.h"
#include "DoubleLinkedList/test_double_linkedlist.h"
#include "Queue/test_queue.h"
#include "Deque/test_deque.h"
#include "CircularLinkedList/test_circular_list.h"
#include "DynamicArray/test_dynamic_array.h"
#include "CString/test_cstring.h"

int main(void) {
    testLinkedList();
    testStack();

    testDoubleLinkedList();
    testQueue();

    testDeque();

    testCircularList();

	testDynamicArray();

	testCString();
    return 0;
}
