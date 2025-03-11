cmake_minimum_required(VERSION 3.10)
project(MyDataStructures C)

# Use at least C11
set(CMAKE_C_STANDARD 11)

# Common warnings; remove "-pedantic" if your compiler doesn't recognize it
set(COMMON_FLAGS "-Wall -Wextra -pedantic")

#
# 1. Build the library
#
add_library(MyDataStructures STATIC
    src/adj_list.c
    src/adj_matrix.c
    src/avl_tree.c
    src/binary_tree.c
    src/bp_tree.c
    src/bst.c
    src/btree.c
    src/circular_list.c
    src/cstring.c
    src/deque.c
    src/double_linkedlist.c
    src/dynamic_array.c
    src/graph.c
    src/hash_table.c
    src/linkedlist.c
    src/pq.c
    src/queue.c
    src/red_black_tree.c
    src/segtree.c
    src/skip_list.c
    src/splay_tree.c
    src/stack.c
    src/trie.c
    src/treap.c
    src/unicode.c

    
)

# Apply compile options to this library
target_compile_options(MyDataStructures PRIVATE ${COMMON_FLAGS})

# Publicly expose the "include" folder so external code can do #include <linkedlist.h>
target_include_directories(MyDataStructures
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link math library if needed (e.g. for floor, pow, etc.)
# target_link_libraries(MyDataStructures PRIVATE m)

#
# 2. Add the tests subdirectory
#    This is where we'll define the single test runner.
#
add_subdirectory(tests)
