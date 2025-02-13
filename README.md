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

Red-Black trees and AVL trees are both self-balancing binary search trees, guaranteeing O(logn) time complexity for insertions, deletions, and lookups. However, they balance themselves in different ways and have slightly different performance characteristics, which can make Red-Black trees preferable in some scenarios:

### Fewer Rotations on Updates

Red-Black trees typically perform at most two rotations to rebalance after an insertion or deletion.
AVL trees, being more rigidly balanced (height-balance), can require several rotations in the worst case for a single insertion or deletion.

If your application involves many insertions and deletions, the fewer rotations in a Red-Black tree can lead to better overall performance.
Easier (in Practice) to Implement

While both data structures can be implemented with standard balancing logic, many programmers find the Red-Black insertion and deletion rules to be a bit more straightforward compared to tracking balance factors and performing the “rotation cascades” typical of AVL trees.

This practical simplicity and conventional use is one reason libraries like the C++ std::map and std::set or Java’s TreeMap are traditionally implemented as Red-Black trees.
Less Strict Height Balancing

An AVL tree maintains a very strict balance condition (the difference in heights of left and right subtrees for any node is at most 1), so it tends to have a smaller height, which is good for lookups but can cause more rotations to maintain that property.

A Red-Black tree’s balance criterion is looser (color-based rules rather than exact height checks), resulting in slightly taller trees on average but fewer, simpler rebalancing operations.

### Good “All-Around” Performance

AVL: Usually has faster lookups if your dataset is fairly static (because its height is more tightly controlled), but insertions and deletions can be more expensive due to extra rotations.

Red-Black: Offers a good balance of performance across all operations—lookups, insertions, and deletions—making it a common default choice when you need a general-purpose self-balancing BST.

### When NOT to Choose a Red-Black Tree Over AVL

If search performance is the absolute priority (e.g., you have very few inserts/deletions but lots of lookups), then AVL might offer a slight edge because it’s more strictly balanced, ensuring a shorter height.

If the rotation cost is not significant in your application and you want the best possible search times, you might prefer an AVL tree.
In practice, however, Red-Black trees are often used as the standard go-to self-balancing BST in many libraries because they provide efficient and predictable performance over a wide range of operations and use cases.


### AVL Tree
![AVL1](diagrams/AVLTree1.png "AVL1")
![AVL2](diagrams/AVLTree2.png "AVL2")
![AVL3](diagrams/AVLTree3.png "AVL3")
![AVL4](diagrams/AVLTree4.png "AVL4")
![AVL5](diagrams/AVLTree5.png "AVL5")
![AVL6](diagrams/AVLTree6.png "AVL6")
![AVL7](diagrams/AVLTree7.png "AVL7")
![AVL8](diagrams/AVLTree8.png "AVL8")
![AVL9](diagrams/AVLTree9.png "AVL9")

### Red–Black Tree
#### 5 Primary Rules:

1. A node is either red or black
2. Root is always black
3. New insertions are always red (and then are recolored depending on the circumstances)
4. Every path from root to leaf has the same number of black nodes.
5. No path can have two consecutive red nodes
6. Nulls are black
#### Rebalancing:
1. If we have a black aunt, we rotate (BAR). After a rotation the three nodes we are working on end up as 
	black
 	/   \
      red   red
2. If we have a red aunt, we color flip. After a color flip the three nodes we are working on end up as
	Red
       /   \
    black  black

https://www.youtube.com/watch?v=nMExd4DthdA&list=PLOXdJ6q8iu4MneI9gySCHiyzAQcveqkIO

![RBT1](diagrams/RBT_1.png "RBT1")
![RBT2](diagrams/RBT_2.png "RBT2")
![RBT3](diagrams/RBT_3.png "RBT3")
![RBT4](diagrams/RBT_4.png "RBT4")
![RBT5](diagrams/RBT_5.png "RBT5")
![RBT6](diagrams/RBT_6.png "RBT6")
![RBT7](diagrams/RBT_7.png "RBT7")
![RBT8](diagrams/RBT_8.png "RBT8")
![RBT9](diagrams/RBT_9.png "RBT9")
![RBT10](diagrams/RBT10.png "RBT10")
![RBT11](diagrams/RBT11.png "RBT11")
![RBT12](diagrams/RBT12.png "RBT12")

### Splay Tree
#### The 'key' to understand 'splaying'
Even if a search for a key fails (the key doesn't exist), splaying brins the node closest to that key to the root. This is beneficial if future searches are near that same key or if you end up inserting that key next (the place it needs to ggo is now near the root). Similarlyy, after you insert a node or delete a node, you splay so that frequent or recent acesses stay near the top.
#### Primitive Rotations
![SplayRotations](diagrams/SplayRotations.png "SplayRotations")

#### 6 differrent cases:
1. Rotate Left
2. Rotate Right
3. Rotate Right Right
4. Rotate Left Left
5. Rotate Right Left
6. Rotate Left Right

#### Examples of Different types of rotations for splay trees:
![Zig](diagrams/Zig.png "Zig")
![ZigZig](diagrams/ZigZig1.png "ZigZig")
![ZigZag](diagrams/ZigZag1.png "ZigZag")

#### Splaying
![SplaySearch1](diagrams/SplaySearch1.png "SplaySearch1")
![SplaySearch2](diagrams/SplaySearch2.png "SplaySearch2")
![SplaySearch3](diagrams/SplaySearch3.png "SplaySearch3")

### Treap
### B-Tree, 
### B+ Tree (often used in databases and filesystems)

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






