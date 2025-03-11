# tests/CMakeLists.txt

include(CTest)

set(TEST_FLAGS "-Wall -Wextra -pedantic")

add_executable(test_runner
    test_runner.c
    test_linkedlist.c
    test_avl.c
    test_binary_tree.c
    test_bp_tree.c
    test_bst.c
    test_btree.c
    test_circular_list.c
    test_cstring.c
    test_deque.c
    test_graph.c
    test_double_linkedlist.c
    test_dynamic_array.c
    test_linkedlist.c
    test_pq.c
    test_queue.c
    test_red_black_tree.c
    test_segtree.c
    test_skip_list.c
    test_splay_tree.c
    test_hash_table.c
    test_queue.c
    test_stack.c
    test_trie.c
    test_treap.c
    test_unicode.c
    # add more test files here
)

target_compile_options(test_runner PRIVATE ${TEST_FLAGS})

target_include_directories(test_runner
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(test_runner PRIVATE MyDataStructures)

add_test(NAME AllTests COMMAND test_runner)
