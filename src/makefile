CC = gcc
CFLAGS = -Wall -Wextra -pedantic

all:
	$(CC) $(CFLAGS) \
	    main.c \
	    LinkedList/linkedlist.c LinkedList/test_linkedlist.c \
	    Stack/stack.c Stack/test_stack.c \
	    DoubleLinkedList/double_linkedlist.c DoubleLinkedList/test_double_linkedlist.c \
	    Queue/queue.c Queue/test_queue.c \
		Deque/deque.c Deque/test_deque.c \
		CircularLinkedList/circular_list.c CircularLinkedList/test_circular_list.c \
		DynamicArray/dynamic_array.c DynamicArray/test_dynamic_array.c \
		-o main

clean:
	rm -f main
