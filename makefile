CC = gcc
CFLAGS = -Wall -Wextra -pedantic

# Default target: normal build (no sanitizer)
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
	    CString/cstring.c CString/test_cstring.c \
	    SkipList/skip_list.c SkipList/test_skip_list.c \
	    HashTable/hash_table.c HashTable/test_hash_table.c \
	    BinaryTree/binary_tree.c BinaryTree/test_binary_tree.c \
	    BinarySearchTree/bst.c BinarySearchTree/test_bst.c \
	    AvlTree/avl_tree.c AvlTree/test_avl.c \
	    RedBlackTree/red_black_tree.c RedBlackTree/test_red_black_tree.c \
	    SplayTree/splay_tree.c SplayTree/test_splay_tree.c \
	    Treap/treap.c Treap/test_treap.c \
	    BTree/btree.c BTree/test_btree.c \
	    BPTree/bp_tree.c BPTree/test_bp_tree.c \
	    PriorityQueue/pq.c PriorityQueue/test_pq.c \
	    Trie/trie.c Trie/test_trie.c \
	    Tools/Unicode/unicode.c Tools/Unicode/test_unicode.c \
	    SegmentTree/segtree.c SegmentTree/test_segtree.c \
		Graph/graph.c Graph/test_graph.c Graph/adj_list.c Graph/adj_matrix.c \
		DataFrame/dataframe.c DataFrame/test_dataframe.c \
	    -o main \
	    -lm

# Build with AddressSanitizer
asan:
	$(CC) $(CFLAGS) -fsanitize=address -g \
	    main.c \
	    LinkedList/linkedlist.c LinkedList/test_linkedlist.c \
	    Stack/stack.c Stack/test_stack.c \
	    DoubleLinkedList/double_linkedlist.c DoubleLinkedList/test_double_linkedlist.c \
	    Queue/queue.c Queue/test_queue.c \
	    Deque/deque.c Deque/test_deque.c \
	    CircularLinkedList/circular_list.c CircularLinkedList/test_circular_list.c \
	    DynamicArray/dynamic_array.c DynamicArray/test_dynamic_array.c \
	    CString/cstring.c CString/test_cstring.c \
	    SkipList/skip_list.c SkipList/test_skip_list.c \
	    HashTable/hash_table.c HashTable/test_hash_table.c \
	    BinaryTree/binary_tree.c BinaryTree/test_binary_tree.c \
	    BinarySearchTree/bst.c BinarySearchTree/test_bst.c \
	    AvlTree/avl_tree.c AvlTree/test_avl.c \
	    RedBlackTree/red_black_tree.c RedBlackTree/test_red_black_tree.c \
	    SplayTree/splay_tree.c SplayTree/test_splay_tree.c \
	    Treap/treap.c Treap/test_treap.c \
	    BTree/btree.c BTree/test_btree.c \
	    BPTree/bp_tree.c BPTree/test_bp_tree.c \
	    PriorityQueue/pq.c PriorityQueue/test_pq.c \
	    Trie/trie.c Trie/test_trie.c \
	    Tools/Unicode/unicode.c Tools/Unicode/test_unicode.c \
	    SegmentTree/segtree.c SegmentTree/test_segtree.c \
		Graph/graph.c Graph/test_graph.c Graph/adj_list.c Graph/adj_matrix.c \
		DataFrame/dataframe.c DataFrame/test_dataframe.c \
		Series/series.c Series/test_series.c \
	    -o main_asan \
	    -lm

clean:
	rm -f main main_asan
