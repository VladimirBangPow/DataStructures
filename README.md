# DataStructures
Data Structures in C


It’s impossible to name all data structures (there are infinitely many variants and specialized forms), but below is a comprehensive overview of many commonly known or fundamental data structures. They are often grouped by the type of operations they support and how they store data. Each bullet below is itself a broad category or a well-defined data structure.

# 1. Linear Data Structures
## Array

A contiguous block of memory storing elements of the same type. Access by index in O(1).
Variants: Dynamic arrays (like C++ std::vector), Static arrays (fixed size).
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

## Queue

Follows FIFO (First-In, First-Out) semantics.
Operations: enqueue, dequeue.
Often implemented with arrays or linked lists.

## Deque (Double-Ended Queue)

Allows insertion and removal at both ends.
Can simulate stacks and queues, and is typically implemented via a linked list or a specialized array structure.
String

Conceptually can be seen as an array of characters (plus a terminator in many languages).
Specialized operations like concatenation, substring, etc.


## Skip List

A layered, probabilistic data structure that allows O(log n) average insertion/search/deletion.
Conceptually built on top of a linked list with additional “express” links to skip over nodes.
Hash Table (a blend of linear and specialized structure)

Stores key–value pairs for average O(1) lookup and insertion, but can degrade to O(n) in worst case.
Common collision handling methods: chaining (linked lists) or open addressing (linear probing, quadratic probing, etc.).

# 2. Trees
## Binary Tree

Each node has up to 2 children (left, right).
Used for hierarchical data, expression parsing, etc.

## Binary Search Tree (BST)

A binary tree enforcing ordering constraints: all keys in the left subtree < node’s key, and all keys in the right subtree > node’s key.
Allows search, insertion, and deletion in O(h) time, where h is tree height.

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






