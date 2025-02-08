# DataStructures
Data Structures in C


It’s impossible to name all data structures (there are infinitely many variants and specialized forms), but below is a comprehensive overview of many commonly known or fundamental data structures. They are often grouped by the type of operations they support and how they store data. Each bullet below is itself a broad category or a well-defined data structure.

# 1. Linear Data Structures
## Dynamic Array

A contiguous block of memory storing elements of the same type or of different types (generic Dynamic Array). Access by index in O(1).

![Dynamic Array](diagrams/DynamicArray.png "Dynamic Array")


## Linked List
### Description
A linked list is a fundamental data structure where each element (node) holds some data and a reference (pointer) to one or more other nodes. Unlike arrays (which store elements contiguously in memory), a linked list’s elements can be scattered throughout memory, with each node “linking” to the next node in the sequence. This structure enables flexible insertion and deletion operations without shifting large blocks of memory.

In the simplest form—a singly linked list—each node has:

Data: The value or payload the node holds (e.g., an integer, string, or any other structure).
Pointer to the next node: A reference (or pointer) to the next node in the sequence.
A standard singly linked list has:

A special pointer called the head that points to the first node.
The last node’s pointer to “next” is typically NULL, indicating the end of the list.

head -> [ data | next ] -> [ data | next ] -> ... -> NULL

### Complexity:

Typically O(n) because you usually have to traverse the list from the beginning until you find the target or reach the end.


## Doubly Linked List: 
Like a linked list except that each node has pointers to both next and previous nodes.

![Double Linked List](diagrams/doubleLinkedList.png "Double Linked List")

## Circular Linked List: 
![Double Linked Circular List](diagrams/circularDoubleLinkedList.png "Circular Double Linked List")

## Stack

Follows LIFO (Last-In, First-Out) semantics.
Operations: push, pop, peek.
Implemented via arrays or linked lists.

![Stack](diagrams/Stack.png "Stack")


## Queue

Follows FIFO (First-In, First-Out) semantics.
Operations: enqueue, dequeue.
Often implemented with arrays or linked lists.

![Queue](diagrams/Queue.png "Queue")

## Deque (Double-Ended Queue)

Allows insertion and removal at both ends.
Can simulate stacks and queues, and is typically implemented via a linked list or a specialized array structure.

![Deque](diagrams/Deque.png "Deque")


## String

Conceptually can be seen as an array of characters (plus a terminator in many languages).
Specialized operations like concatenation, substring, etc.

### Common C String Functions

Below is a list of commonly used C string functions from the `<string.h>` library:

---

#### 1. `strlen`
- **Description**: Calculates the length of a string (number of characters) excluding the null terminator (`\0`).
- **Syntax**: `size_t strlen(const char *str);`
- **Example**: 
  ```c
  char str[] = "Hello";
  size_t len = strlen(str); // len = 5

#### 2. `strcpy`
- **Description**: Copies the contents of one string (including the null terminator) to another
- **Syntax**: `char *strcpy(char *dest, const char *src);`
- **Example**: 
  ```c
  char str[] = "Hello";
  char dest[10];
  strcpy(dest, src); //dest = "Hello"

#### 3. `strncpy`
- **Description**: Copies up to n chars from one string to another. If src is shorter than n, the remaining chars in dest are padded with null bytes
- **Syntax**: `char *strncpy(char *dest, const char *src, size_t n);`
- **Example**: 
  ```c
  char str[] = "Hello";
  char dest[10];
  strncpy(dest, src, 3); //dest = "Hel"


#### 4. `strcat`
- **Description**: Appends (concatenates) one string to the end of another
- **Syntax**: `char *strcat(char *dest, const char *src);`
- **Example**: 
  ```c
  char dest[20] = "Hello";
  char src[] = " World";
  strcat(dest, src); //dest = "Hello World"

#### 5. `strncat`
- **Description**: Appends up to n charachters from one string to the end of another
- **Syntax**: `char *strncat(char *dest, const char *src, size_t n);`
- **Example**: 
  ```c
  char dest[20] = "Hello";
  char src[] = " World";
  strcat(dest, src, 3); //dest = "Hello Wo"

#### 6. `strcmp`
- **Description**: Compares two strings lexicographically. Returns 0 if the strings are equal. A negative value if str1 is less than str2. A positive value if str1 is greater than str2.
- **Syntax**: `int strcmp(const char *str1, const char *str2);`
- **Example**: 
  ```c
  char str1[] = "Hello";
  char str2[] = "World";
  int result = strcmp(str1, str2); //result < 0

#### 7. `strncmp`
- **Description**: Compares up to n characters of two strings lexicographically.
- **Syntax**: `int strncmp(const char *str1, const char *str2, size_t n);`
- **Example**: 
  ```c
  char str1[] = "Hello";
  char str2[] = "Heaven";
  int result = strcmp(str1, str2, 2); //result = 0 (first 2 characters are equal)

#### 8. `strchr`
- **Description**: Finds the first occurrence of a character in a string. Returns a pointer to the character or NULL if not found
- **Syntax**: `char *strchr(const char *str, int c);`
- **Example**: 
  ```c
  char str[] = "Hello";
  char *ptr = strchr(str, 'e'); //ptr points to 'e'

#### 9. `strrchr`
- **Description**: Finds the last occurrence of a character in a string. Returns a pointer to the character or NULL if not found
- **Syntax**: `char *strrchr(const char *str, int c);`
- **Example**: 
  ```c
  char str[] = "Hello";
  char *ptr = strrchr(str, 'l'); //ptr points to the second 'l'

#### 10. `strstr`
- **Description**: Finds the first occurrence of a substring in a string. Returns a pointer to the beginning of the substring or NULL if not found.
- **Syntax**: `char *strstr(const char *haystack, const char *needle);`
- **Example**: 
  ```c
  char str[] = "Hello World";
  char *ptr = strstr(str, 'World'); //ptr points to 'World'

#### 11. `strtok`
- **Description**: Splits a string into tokens based on a set of delimiters. Modifies the original string by inserting null terminators.
- **Syntax**: `char *strtok(char *str, const char *delim);`
- **Example**: 
  ```c
  char str[] = "Hello,World,How,Are,You";
  char *token = strtok(str, ",");
  while (token != NULL){
	printf("%s\n", token);
	token = strtok(NULL, ",");
  }

## Skip List

![Skip List 1](diagrams/SkipList1.png "Skip List 1")
![Skip List 2](diagrams/SkipList2.png "Skip List 2")
![Skip List 3](diagrams/SkipList3.png "Skip List 3")
![Skip List 4](diagrams/SkipList4.png "Skip List 4")


A layered, probabilistic data structure that allows O(log n) average insertion/search/deletion.
Conceptually built on top of a linked list with additional “express” links to skip over nodes.

`insert(SkipList *sl, void *data)` The way the insert algorithm works. First we create an update list, which will help us in the last step when going to update the references and insert the new node. The goal of the algorithm is to create this update list and then use it. So first we want to skip over the top level to a node that is right before our possible insertion point. Then we move down and skip from that same node (current is used at multiple levels). This means that our skipping is leading to log(n) performance, because we are carrying the node over to the next levels. Every level of the structure we want to update the last node we skipped to into our update list. So at level 2 update[1]=current after we skipped current over. At level 1 update[0]=current after we skipped, etc. Once we have updated update[], we then want to check if the data in our skip list contains the insertion data. This means we update current one last time to current = current->forward[0]. Current could be Null at this point, but it might contain the exact data we are trying to insert. We check and return if so, otherwise we no longer need current as our update table remembers how far we skipped at each level. Now we need to flip a coin to see how many nodes are to be inserted in a tower like structure. We update sl->level and add new levels as needed to update[newLevel]. Next we create the data for the new node. Finally we use the update list to insert the new node at each level, updating references.

## Hash Table (a blend of linear and specialized structure)

![Hash Table](diagrams/HashTable.png "HashTable")

Stores key–value pairs for average O(1) lookup and insertion, but can degrade to O(n) in worst case.
Common collision handling methods: chaining (linked lists) or open addressing (linear probing, quadratic probing, etc.).

# 2. Trees
## Binary Tree

Each node has up to 2 children (left, right).
Used for hierarchical data, expression parsing, etc.

## Binary Search Tree (BST)

A binary tree enforcing ordering constraints: all keys in the left subtree < node’s key, and all keys in the right subtree > node’s key.
Allows search, insertion, and deletion in O(h) time, where h is tree height.
![BST1](diagrams/BinarySearchTree1.png "BST1")
![BST2](diagrams/BinarySearchTree2.png "BST2")
![BST3](diagrams/BinarySearchTree3.png "BST3")
![BST4](diagrams/BinarySearchTree4.png "BST4")

## Balanced Trees

Maintain balanced height for faster lookups (O(log n)). Examples:
AVL Tree
Red–Black Tree
Splay Tree
Treap
B-Tree, B+ Tree (often used in databases and filesystems)

## Heap/Priority Queue

A specialized tree-based structure (often represented implicitly via an array) where the parent’s key is either always larger (max-heap) or smaller (min-heap) than its children.
Supports efficient retrieval of min/max in O(1) and insertion/deletion in O(log n).

## Trie (Prefix Tree)

A tree specialized for storing strings by character.
Each edge typically represents one character. Enables fast prefix lookups.

## Segment Tree

Specialized tree for storing information about intervals, segments (e.g., sums over array ranges).
Allows O(log n) queries and updates on intervals.

## Fenwick Tree (Binary Indexed Tree)

A compact structure for cumulative frequency tables or prefix sums.
Supports O(log n) update and prefix-sum queries.
Disjoint Set (Union-Find)

Not a tree in the traditional sense, but often implemented with “forest” of trees.
Tracks elements partitioned into a number of disjoint (non-overlapping) sets. Supports fast union and find operations.

# 3. Graphs and Graph Representations
## Graph (in general)

A set of vertices (nodes) connected by edges (links).
Can be directed, undirected, weighted, or unweighted.

## Adjacency List

Each node has a list of its neighbors. More efficient for sparse graphs.

## Adjacency Matrix

A 2D matrix (size V×V) indicating existence/weight of edges. Easier for dense graphs or if you need fast existence checks.

## Incidence List / Incidence Matrix

Less commonly used but sometimes helpful for certain algorithms.

# 4. Specialized/Advanced Data Structures
## Suffix Tree

A compressed trie of all suffixes of a given string. Enables fast substring queries.
Construction can be O(n) with advanced algorithms (e.g. Ukkonen’s).

## Suffix Array

An array of all suffixes of a string in sorted order, often combined with additional structures (e.g., LCP array).
Requires less memory than a suffix tree, but certain queries can be slightly slower.

## Bloom Filter

A probabilistic data structure to test membership in a set. Can yield false positives but never false negatives.
Extremely space efficient for large sets with a known false-positive rate.

## LRU Cache

Conceptually a specialized structure combining a hash map + doubly linked list to track the least recently used items for quick eviction.

## R-Tree (and variants)

A tree for spatial data (rectangles, polygons, etc.). Useful in GIS and spatial indexing.

## Quadtree, Octree

Multi-level tree where each node can have 4 (quadtree) or 8 (octree) children for partitioning 2D or 3D space respectively.

## Order Statistic Tree

A self-balancing BST that also keeps track of subtree sizes, enabling rank queries (“what is the k-th smallest element?”) in O(log n) time.

## Van Emde Boas Tree

A tree-like structure for integer keys in a bounded universe. Achieves O(log log M) operations for a universe of size M.

## Cartesian Tree, Treap

Hybrids of BST + heap properties for specialized use.
# 5. Persistent Data Structures
Versions of lists, trees, tries, etc. that keep previous versions of themselves immutable while still allowing new updates.
Uses structural sharing to avoid copying entire data structures on each modification.

# Choosing the Right Data Structure
## Constraints: How large can the data get? How many operations do you need to perform, and how fast do they need to be?
## Operations: Do you need random access, or is sequential enough? Do you need to search, insert, delete quickly, or do range queries, or do merges?
## Memory: Are you memory-constrained (e.g., small embedded system) or do you have plenty of RAM?
While it’s not practical to list every data structure ever conceived (there are many domain-specific or hybrid variations), this overview should give you a sense of the major categories and common data structures you’ll encounter in computer science and software development.






