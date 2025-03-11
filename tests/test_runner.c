#include "include/test_linkedlist.h"
#include "include/test_stack.h"
#include "include/test_double_linkedlist.h"
#include "include/test_queue.h"
#include "include/test_deque.h"
#include "include/test_circular_list.h"
#include "include/test_dynamic_array.h"
#include "include/test_cstring.h"
#include "include/test_skip_list.h"
#include "include/test_hash_table.h"
#include "include/test_binary_tree.h"
#include "include/test_bst.h"
#include "include/test_avl.h"
#include "include/test_red_black_tree.h"
#include "include/test_splay_tree.h"
#include "include/test_treap.h"
#include "include/test_btree.h"
#include "include/test_bp_tree.h"
#include "include/test_pq.h"
#include "include/test_trie.h"
#include "include/test_unicode.h"
#include "include/test_segtree.h"
#include "include/test_graph.h"

int main(void) {
    testLinkedList();
    testStack();
    testDoubleLinkedList();
    testQueue();
    testDeque();
    testCircularList();
	testDynamicArray();
	testCString();
    testSkipList();
    testHashTable();
    testBinaryTree();
    testBinarySearchTree();
    testAvlTree();
	testRedBlackTree();
	testSplayTree();
	testTreap();
	testBTree();
    testBPTree();
    testPriorityQueue();
    testUnicode();
    testTrie();
    testSegTree();
    testGraph();
    return 0;
}
