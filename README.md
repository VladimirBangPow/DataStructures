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


## AVL Tree
![AVL1](diagrams/AVLTree1.png "AVL1")
![AVL2](diagrams/AVLTree2.png "AVL2")
![AVL3](diagrams/AVLTree3.png "AVL3")
![AVL4](diagrams/AVLTree4.png "AVL4")
![AVL5](diagrams/AVLTree5.png "AVL5")
![AVL6](diagrams/AVLTree6.png "AVL6")
![AVL7](diagrams/AVLTree7.png "AVL7")
![AVL8](diagrams/AVLTree8.png "AVL8")
![AVL9](diagrams/AVLTree9.png "AVL9")

## Red–Black Tree
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

## Splay Tree
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

## Treap
A Treap is a binary search tree on keys and a heap on random priorities. After each standard BST insertion, rotations ensure that a node’s parent always has a larger or equal priority. Because priorities are random, the tree is balanced in expectation with O(log n) average operation time.

#### BST property (by key):

For any node, all keys in its left subtree are smaller than the node’s key, and all keys in its right subtree are larger.
Heap property (by priority):

Each node also has a priority (often a random number).
For a max-heap property, for example, a node’s priority is greater than or equal to the priorities of its children.

In practice, the priority is usually chosen randomly when a node is inserted.
When you insert a node, you:

First insert it into the BST by key (as if you were inserting into a regular BST).
Then “rotate” the node up or down to fix any violation of the heap property (i.e., making sure each node’s priority remains higher or equal to its children if using a max-heap convention).
#### When you delete a node, you:

“Rotate” the node down until it becomes a leaf (fixing the heap property by rotating it towards a direction that maintains the priority structure).
Then remove it from the tree.

#### Why randomize?
By assigning random priorities to each node and imposing the heap property, the tree’s shape tends to be balanced on average. Specifically, the expected height of a treap is O(log n) with high probability, leading to average O(log n) times for operations such as search, insert, and delete.

#### Key Takeaways
A treap is a BST with respect to the key and a heap with respect to the priority.
The priorities are typically random, which ensures a high probability of balanced trees without complex balancing steps used by other self-balancing BSTs (like AVL or Red-Black Trees).
All main operations (search, insert, delete) can be performed in O(log n) expected time.

#### Insertion Example

![Treap1](diagrams/Treap1.png "Treap1")
![Treap2](diagrams/Treap2.png "Treap2")
![Treap3](diagrams/Treap3.png "Treap3")
![Treap4](diagrams/Treap4.png "Treap4")
![Treap5](diagrams/Treap5.png "Treap5")
![Treap6](diagrams/Treap6.png "Treap6")

#### Deletion Example
Treap deletion rules:
1. Find the node via standard BST search (compare keys, go left/right)
2. If the node has:
   - 0 children (leaf): just remomve it
   - 1 child: remove the node and link the child up
   - 2 children: rotate the node down (left or right) until it becmes a case of 0 or 1 child, then remove it.
     	The rotation direction depends on the priorities of the child subtrrees. If left->priority > right->priority, rotate right. Else, rotate left. This pushes down the node to be deleted while keeping the heap property intact on each step.

![Treap7](diagrams/Treap7.png "Treap7")
![Treap8](diagrams/Treap8.png "Treap8")  
     
## B-Tree
A B-tree is a self-balancing tree data structure commonly used in databases and file systems to store and manage large volumes of data efficiently. Unlike traditional binary search trees, which have at most two children per node, B-trees can have many children per node (often called "branches" or "subtrees"). This multi-way branching design enables B-trees to maintain shallow height even with large data sets, leading to fewer disk or I/O operations—an especially important consideration in database and storage systems.

#### Here are some key points about B-trees:

1. Multi-way branching:
- Each node can have multiple keys (or values) and can point to multiple children.
- The number of keys and children in a node is constrained by a parameter typically called the order or minimum degree of the B-tree.

2. Height is kept small:
- Because each node can hold multiple keys, the tree’s height grows more slowly than a regular binary tree.
- This reduces the number of disk accesses needed to find or insert data.

3. Balanced structure:
- B-trees enforce balance by ensuring that each non-root node has at least a certain minimum number of keys and children, and up to a defined maximum.
- As new keys are inserted or old keys are deleted, nodes split or merge to maintain balance, preventing the tree from becoming too tall or skewed.

4. Efficient insertion, deletion, and search:
- All of these operations can be performed in O(log n) time.
- B-trees are designed for scenarios where slow disk access (or other forms of high-latency storage access) dominates performance.
- By minimizing the number of disk reads/writes, B-trees achieve better overall performance.
5. Common uses:
- Database indexing (e.g., many database management systems use B-trees or variants like B+ trees to store index data).
- File systems (often used to store directory information, file metadata, and block indices).
- Key–value stores and other high-performance storage engines.

6. Variants of B-trees:
- B+ tree: A popular variant where all values (records) are stored in the leaf nodes, and internal nodes store only keys used for navigation. This design can provide better range query performance.
- B-tree*, B#-tree, etc.: Variants focusing on different performance characteristics or implementation details.

Overall, the B-tree’s design is tailored for environments where data must be read from and written to large, slow storage blocks (such as disk pages). By storing multiple keys per node and keeping the tree height small, B-trees reduce expensive I/O operations, making them ideal for large-scale data storage systems.


#### In a B-Tree, we need to differentiate between leaf nodes and internal nodes for several reasons:

1. Traversal Logic

- When searching for a key, if you reach a leaf node and still haven’t found the key, you know the key is not in the tree.
- Conversely, if a node is not a leaf (i.e., an internal node), you know there are child pointers you can (and should) follow to continue your search.
2. Insertion and Split Rules

- Insertion behaves differently depending on whether the node is a leaf or internal.
- In a leaf node, you can directly insert the new key (if there is room).
- In an internal node, you typically descend into a child. If that child is full, you split it first, then move down to the correct child.
- Knowing if a node is a leaf is critical for deciding these steps.
3. Deletion Logic

- When deleting a key, if it is found in a leaf, you simply remove it.
- If found in an internal node, you have to swap with a predecessor (or successor) key or merge children.
- The deletion algorithm for B-Trees is more involved, but it always checks whether a node is leaf to decide which case applies.
4. Implementation Simplification

- Marking a node as leaf = true or false makes the code simpler: you don’t have to guess if the children[] array is valid or not.
- A leaf node has no valid children, whereas an internal node must keep track of them.
- This single boolean flag allows quick checks anywhere in the code to determine how to handle that node.

So, the leaf flag makes it efficient and clear to handle B-Tree nodes differently when they do or do not contain child pointers.

#### Important Note on splitting:
When you insert, you start at the root and mmove down through the children until you reach the appropriate leaf. If the root is full from the start, you'll eventually have to split it anyway. It's easierr and cleaner to split before descending. After the root is split, the newly created root will have fewerr keys, and you can keep descending to the correct leaf child, splitting chhild nodes as needed along the way.

#### Important Note on Searching:
We use a linear scan for the key:
`for(i=0; i<cur->nkeys && tree->cmp(keyy, cur->keys[i])>0; i++);`
The loop advances i until one of the two conditions is met:
1. We have checked all cur->nkeys
2. We encounter a key that is greater than or equal to key.

Next we check if we encountered the key

`
if (i<cur->nkeys && tree->cmp(key, cur->keys[i])==0){return cur->keys[i];}
`

`
else if (cur->leaf){return NULL;}
`

`
else {cur=cur->children[i];}
`

The final statement is of incredible importance:
1. It shows how the B-Tree is structured.
2. If we stop on the i'th key in the key list, then we also descend into the i'th child.
3. To understand further, you would need to understand how the insert function works for B-Tree
4. Remember B-Tree's have nodes that have between t-1 and 2t-1 keys, and the number of children = (number of keys)+1
5. If a node has n keyys, they act as "dividers" splittingg the key space into n+1 regions. Each gap corresponds to one child pointer

#### Important Note on Insertion:
1. When we split the full child, we are never actually zero'ing out the key locations that move to new nodes, we are merely keeping track of nkeys of the old child. This is because we essentially copy the right half of the full child into a new child, then we promote the middle key, and keep the rest (to the left) in the old child. That's why we don't have to erase anything.

![BTree1](diagrams/BTree.png "BTree1")
![BTree2](diagrams/BTree2.png "BTree2")
![BTree3](diagrams/BTree3.png "BTree3")
![BTree4](diagrams/BTree4.png "BTree4")
![BTree5](diagrams/BTree5(2).png "BTree5")
![BTree6](diagrams/BTree6(2).png "BTree6")
![BTree7](diagrams/BTree7(2).png "BTree7")
![BTree8](diagrams/BTree8(2).png "BTree8")
![BTree9](diagrams/BTree9(2).png "BTree9")
![BTree10](diagrams/BTree10(2).png "BTree10")
![BTree11](diagrams/BTree11.png "BTree11")
![BTree12](diagrams/BTree12.png "BTree12")
![BTree13](diagrams/BTree13.png "BTree13")
![BTree14](diagrams/BTree14.png "BTree14")
![BTree15](diagrams/BTree15.png "BTree15")


## B+ Tree (often used in databases and filesystems)
A B+ tree is a specialized data structure often used in databases and filesystems to maintain sorted data in a way that supports efficient insertion, deletion, and lookup operations (especially for range queries). It is an extension of the B-tree data structure, with some key differences that make it particularly well-suited for disk-based or other secondary-storage-based indexing.

### Key Characteristics
1. Balanced Tree Structure

- Like B-trees, B+ trees are balanced, meaning all leaves appear at the same level (the height of every leaf is the same).
- This ensures that searches, insertions, and deletions take O(logn) time in terms of the number of elements n.

2. Nodes Have a Range of Children
   
- B+ trees, like B-trees, have an order (often denoted as m), which specifies the maximum number of children a node can have.
- Each non-leaf (internal) node can have between ⌈m/2⌉ and m children (except possibly the root which can have fewer).
- Each leaf node can also hold a certain range of record pointers or data entries.

3. Separation of Internal Keys and Data
   
- Internal nodes (also referred to as “index nodes”) only store keys (up to m-1) that act as separators (guides) to direct the search.
- Leaf nodes store the actual data values (in the form of record pointers or references to the data).
- This is the key difference from a standard B-tree, where both internal nodes and leaves can store actual data.

4. Linked Leaves
   
- In a B+ tree, all leaf nodes are linked together in a linked list (often referred to as a horizontal link or leaf chain).
- This allows efficient range queries, because once a search identifies the starting leaf, the linked list can be traversed to retrieve subsequent values without having to climb back up into the internal tree structure.

5. High Fan-Out

- Because nodes can hold multiple keys and children, B+ trees have a high fan-out.
- This reduces the height of the tree and typically optimizes for disk-based block I/O by minimizing the number of pages/blocks that need to be read.


### Structure of a B+ Tree
Let’s consider a B+ tree of order m. The structure follows certain rules:

1. Root Node

- Special cases apply to the root node. The root can have fewer than ⌈m/2⌉ children if it is not full or if the tree has few elements. However, beyond a minimal threshold, it follows the same constraints as other internal nodes.


2. Internal Nodes

- Each internal node contains up to m−1 keys and up to m child pointers.
- Each key in an internal node serves as a separator defining ranges for the child pointers. For example, if an internal node has keys [𝑘1,𝑘2,…,𝑘𝑛], the pointers between and around these keys direct the search for values less than 𝑘1, between 𝑘1 and 𝑘2, etc.

3. Leaf Nodes

- All actual data records or pointers to data are stored in the leaf nodes.
- Each leaf node can hold between ⌈𝑚/2⌉ and m entries (depending on the exact variant of the B+ tree).
- Each leaf node has a pointer to the next leaf node (and optionally a pointer to the previous leaf node for a doubly linked structure). This is crucial for efficient range queries.

### Operations
1. Search (Lookup)
- Start from the root node.
- Compare the search key with the keys in the current node to find the correct child pointer to follow.
- Move down the tree level by level until you reach a leaf node.
- Within the leaf node, search through its entries for the specific key (if it exists).
- Return the record pointer or indication that the key was not found.
Time Complexity: 𝑂(log𝑛) due to the balanced tree property.

2. Insertion
- Search for the correct leaf node where the new key should be inserted.
- Insert the key into the leaf node (keeping the leaf node’s keys in sorted order).
- If the leaf node does not overflow (i.e., does not exceed m entries), the insertion is complete.
- If the leaf node overflows:
	- Split the leaf into two nodes, typically around the median key.
	- Promote the middle key (or a separator key) to the parent internal node.
	- If the parent internal node also overflows, recursively split and promote further up.
	- If the split reaches the root (and the root overflows), create a new root node and increase the tree height by one.
Time Complexity: 𝑂(log𝑛)
![BP1](diagrams/BP1.png "BP1")
![BP2](diagrams/BP2.png "BP2")
![BP3](diagrams/BP3.png "BP3")
![BP4](diagrams/BP4.png "BP4")
![BP5](diagrams/BP5.png "BP5")
![BP6](diagrams/BP6.png "BP6")
![BP7](diagrams/BP7.png "BP7")
![BP8](diagrams/BP8.png "BP8")
![BP9](diagrams/BP9.png "BP9")
![BP10](diagrams/BP10.png "BP10")
![BP11](diagrams/BP11.png "BP11")


3. Deletion
- Search the leaf node containing the key to be deleted.
- Remove the key from the leaf node.
- If the leaf node still has enough entries (at least ⌈𝑚/2⌉), no further action is required.
- If it underflows (i.e., has fewer entries than ⌈𝑚/2⌉):
	- Attempt to “borrow” an entry from a sibling node if the sibling has extra.
	- If borrowing is not possible, merge the leaf with a sibling, effectively reducing the number of leaf nodes by one.
	- Update or remove the corresponding separator key in the parent internal node.
	- If the parent underflows, recursively handle it in the same manner (borrow from or merge with sibling).
	- If merging/splitting occurs at the root and results in underflow, the root can be adjusted.
Time Complexity: 𝑂(log𝑛) similarly to insertion.
![BP12](diagrams/BP12.png "BP12")
![BP13](diagrams/BP13.png "BP13")


NOTE: Works on GCC but not CLANG compiler. This is definitely a bad bug, but I can't really fix the compiler. I will try and look for a workaround in the code, but no promises. Just use GCC for now.

## Heap/Priority Queue

A specialized tree-based structure (often represented implicitly via an array) where the parent’s key is either always larger (max-heap) or smaller (min-heap) than its children.
Supports efficient retrieval of min/max in O(1) and insertion/deletion in O(log n).

Min-heaps are specialized data structures that always give you quick access to the smallest (or “minimum”) element in a collection. Because of this guaranteed access to the minimum, min-heaps are commonly used in any scenario where you repeatedly need to extract the smallest item or reorder items based on priority (where “priority” is usually defined in ascending order). Below are some of the most common applications:

A priority queue is an abstract data type where each element has a "priority." A min-heap efficiently implements a min-priority queue, meaning you can always remove (pop) the smallest element first.
Common usage examples include job scheduling where tasks with the highest priority (often the lowest “priority number” means urgent) are processed first.

### Graph Algorithms (e.g., Dijkstra’s Algorithm)

In Dijkstra’s algorithm for shortest paths in a weighted graph, you repeatedly pick the vertex with the smallest tentative distance and relax (update) its neighbors. A min-priority queue (min-heap) is typically used to retrieve the next closest vertex efficiently.
Similarly, Prim’s algorithm for minimum spanning trees also uses a min-heap to select the next edge of minimum weight.

### Huffman Coding

Huffman’s algorithm builds an optimal prefix code (often used in data compression). It repeatedly merges the two smallest-frequency nodes (from a priority queue) into a combined node. A min-heap makes it efficient to find and remove the smallest two frequencies each time.
Order Statistics

If you only need to track and repeatedly extract the smallest k elements in a streaming fashion, a min-heap can help. (Or in some cases, people use max-heaps to keep track of largest elements — but the heap approach is similar.)

### Online Median Finding (in tandem with a max-heap)

A classic solution for finding a running median uses a max-heap for the lower half of numbers and a min-heap for the upper half. Balancing these two heaps lets you query the median in O(1) time and insert new elements in O(
logn) time.

### Scheduling Problems

In operating systems or real-time systems, tasks/jobs often have priorities, and you may need to pick the highest-priority (lowest numeric value) job next. A min-heap is well-suited for this type of scheduling queue.

### Merging k Sorted Lists

When you have multiple sorted lists and want to merge them into one sorted list (for example in external sorting or in certain types of database queries), you can keep one element from each list in a min-heap. You always extract (pop) the smallest of these, then insert the next element from the list that was just taken.
In general, whenever you need quick extraction of the smallest element (in approximately O(logn) time), or you need to insert new elements while preserving a structure where the smallest can be found/removed efficiently, a min-heap is a perfect fit.

### Pushing
![Pq1](diagrams/Pq1.png "Pq1")
![Pq2](diagrams/Pq2.png "Pq2")
![Pq3](diagrams/Pq3.png "Pq3")
![Pq4](diagrams/Pq4.png "Pq4")
![Pq5](diagrams/Pq5.png "Pq5")

### Popping
![Pq6](diagrams/Pq6.png "Pq6")
![Pq7](diagrams/Pq7.png "Pq7")

## Trie (Prefix Tree)

A tree specialized for storing strings by character.
Each edge typically represents one character. Enables fast prefix lookups.

![Trie1](diagrams/Trie11.png "Trie1")
![Trie2](diagrams/Trie12.png "Trie2")
![Trie3](diagrams/Trie13.png "Trie3")
![Trie4](diagrams/Trie14.png "Trie4")
![Trie5](diagrams/Trie15.png "Trie5")
![Trie6](diagrams/Trie16.png "Trie6")
![Trie7](diagrams/Trie17.png "Trie7")
![Trie8](diagrams/Trie18.png "Trie8")



## Segment Tree

Specialized tree for storing information about intervals, segments (e.g., sums over array ranges).
Allows O(log n) queries and updates on intervals.

A segment tree is a specialized binary tree data structure that stores information about intervals (or segments) of an array. It allows you to efficiently query and update information over a range of indices, making it very useful for problems that require repeated range queries and updates.

### Key Ideas
#### Divide-and-Conquer Approach:
- The array is split into two halves recursively until each segment represents a single element.
- Each node in the tree covers a specific segment of the array, typically identified by a range [𝐿,𝑅]

#### Storing Aggregated Information:
- Each node stores some information about the segment it represents—common examples include:
	- Sum of the elements in the segment
 	- Minimum or Maximum value in the segment
  	- Greatest Common Divisor (GCD) of the segment
- These values in the parent node can be computed by combining values from its children.

#### Fast Queries:

- Once built, a segment tree can answer queries like
	- “What is the sum of the elements in the range [𝑖,𝑗]?” or
 	- “What is the minimum value in [𝑖,𝑗]?”
  	- in O(log𝑛) time, where 𝑛 is the size of the array.

#### Efficient Updates:

In addition to queries, segment trees handle updates (like changing the value of a single element) in O(log𝑛) time as well.
After updating a leaf node, you only need to recalculate the information up the tree path to the root.

#### Space Complexity:

A segment tree typically requires about 4n space in the worst case (where 𝑛 is the size of the array), which is still considered 𝑂(𝑛).

![Seg1](diagrams/SegmentTree1.png "Seg1")
![Seg2](diagrams/SegmentTree2.png "Seg2")
![Seg3](diagrams/SegmentTree3.png "Seg3")
![Seg41](diagrams/SegmentTree41.png "Seg41")
![Seg5](diagrams/SegmentTree5.png "Seg5")


# 3. Graphs and Graph Representations
## Graph (in general)

A set of vertices (nodes) connected by edges (links).
Can be directed, undirected, weighted, or unweighted.

### Undirected Unweighted Graph
![UndirectedUnweightedGraph](diagrams/UndirectedUnweightedGraph.png "UndirectedUnweightedGraph")


### Undirected Weighted Graph
![UndirectedWeightedGraph](diagrams/UndirectedWeightedGraph.png "UndirectedWeightedGraph")

### Directed Unweighted Graph
![DirectedUnweightedGraph](diagrams/DirectedUnweightedGraph.png "DirectedUnweightedGraph")


### Directed Weighted Graph
![DirectedWeightedGraph](diagrams/DirectedWeightedGraph.png "DirectedWeightedGraph")


### Building A Graph with an Adjacency List:

![GraphBuild1](diagrams/GraphBuild1.png "GraphBuild1")
![GraphBuild2](diagrams/GraphBuild2.png "GraphBuild2")
![GraphBuild3](diagrams/GraphBuild3.png "GraphBuild3")
![GraphBuild4](diagrams/GraphBuild4.png "GraphBuild4")

### Building A Graph with an Adjacency Matrix:
![GraphBuild5](diagrams/GraphBuild5.png "GraphBuild5")
![GraphBuild6](diagrams/GraphBuild6.png "GraphBuild6")
![GraphBuild7](diagrams/GraphBuild7.png "GraphBuild7")


In essence, the core logic of BFS, DFS, or Dijkstra’s remains the same regardless of whether the graph is directed or undirected, weighted or unweighted. However, there are small but important adjustments in how you apply each algorithm to different graph types:

### 1. BFS
- Purpose: Typically used for unweighted graphs (or graphs where every edge has the same cost) to find the shortest path in terms of the number of edges, or to explore all connected vertices from a starting point.

#### Undirected vs. Directed

- In an undirected graph, you follow edges in both directions automatically.
- In a directed graph, you only follow edges in the forward direction (i.e., if there’s an edge 𝑢→𝑣, but not 𝑣→𝑢, you only enqueue v when you’re at 𝑢).

#### Weighted vs. Unweighted

- BFS inherently doesn’t handle varying weights.
- If all edges have the same weight (e.g., weight = 1), BFS still finds the shortest path in terms of “number of edges” (or total uniform cost).
- If the graph has different weights, BFS is no longer suitable for shortest paths, and you’d typically use Dijkstra or another weighted shortest-path algorithm.

![AdjacencyListBFS1](diagrams/AdjacencyListBFS1.png "AdjacencyListBFS1")
![AdjacencyListBFS2](diagrams/AdjacencyListBFS2.png "AdjacencyListBFS2")
![AdjacencyListBFS3](diagrams/AdjacencyListBFS3.png "AdjacencyListBFS3")
![AdjacencyListBFS4](diagrams/AdjacencyListBFS4.png "AdjacencyListBFS4")
![AdjacencyListBFS5](diagrams/AdjacencyListBFS5.png "AdjacencyListBFS5")
![AdjacencyListBFS6](diagrams/AdjacencyListBFS6.png "AdjacencyListBFS6")


##### Summary: The BFS procedure itself does not change much—just respect the direction of edges if the graph is directed. If the graph is weighted with different costs, BFS won’t give the true minimal-cost path.

### 2. DFS
- Purpose: Depth-first search is used mostly to explore or check connectivity, detect cycles, or do topological sorts (in directed acyclic graphs), etc. It doesn’t care about edge weights when simply exploring.

#### Undirected vs. Directed
- In an undirected graph, you can move both ways between connected vertices.
- In a directed graph, you only traverse in the allowed direction.

#### Weighted vs. Unweighted

- DFS makes no use of weights in the standard algorithm.
- You can ignore weights or treat them as present but irrelevant.

![AdjacencyListDFS1](diagrams/AdjacencyListDFS1.png "AdjacencyListDFS1")
![AdjacencyListDFS2](diagrams/AdjacencyListDFS2.png "AdjacencyListDFS2")
![AdjacencyListDFS3](diagrams/AdjacencyListDFS3.png "AdjacencyListDFS3")
![AdjacencyListDFS4](diagrams/AdjacencyListDFS4.png "AdjacencyListDFS4")



##### Summary: DFS is unaffected by weights. The only difference between undirected and directed is whether you consider edges in both directions (undirected) or in the specified direction only (directed).

### 3. Dijkstra’s Algorithm
- Purpose: Finds the shortest path in a graph where edges have nonnegative weights.

#### Undirected vs. Directed

- Conceptually, Dijkstra’s logic is the same:
- Keep track of a “distance” array.
- Use a priority queue (min-heap) to pick the next closest vertex.
- Update (relax) distances to neighbors.
- In an undirected graph, each undirected edge (𝑢,𝑣) is stored internally as two directed edges 𝑢→𝑣 and 𝑣→𝑢.
- The algorithm still relaxes edges from each node to its neighbors.
- In a directed graph, edges are only in one direction.
- You only relax edges 𝑢→𝑣 if they exist.

#### Weighted vs. Unweighted

- Weighted (nonnegative): Dijkstra is the standard choice to get the minimal total cost path.
- Unweighted: Dijkstra still “works” but is overkill—BFS is typically simpler and more efficient if all edges have the same cost (like weight = 1).
- If there are negative edge weights, you need a different algorithm (e.g., Bellman-Ford).

![Dijkstra1](diagrams/Dijkstra1.png "Dijkstra1")
![Dijkstra2](diagrams/Dijkstra2.png "Dijkstra2")
![Dijkstra3](diagrams/Dijkstra3.png "Dijkstra3")
![Dijkstra4](diagrams/Dijkstra4.png "Dijkstra4")
![Dijkstra5](diagrams/Dijkstra5.png "Dijkstra5")
![Dijkstra6](diagrams/Dijkstra6.png "Dijkstra6")




##### Summary: The Dijkstra procedure is the same. The difference is in how edges are stored (two directions for undirected vs. one for directed) and whether each edge has a real cost or a uniform cost.

### Bottom Line
- The fundamental BFS/DFS logic doesn’t really change; you just decide whether an edge (𝑢,𝑣)
- (u,v) exists in one direction or both.
- Weights do not affect BFS or DFS unless all weights are the same (in which case BFS can still find the shortest path in “hops”) or you are ignoring weights entirely (typical for DFS).
- Dijkstra is always the same “relaxation + priority queue” approach.
- The only variation is in how you build your adjacency list or matrix for directed vs. undirected edges.
- So the algorithms themselves are conceptually the same across the different graph “types.” The real difference is whether or not you:
- Consider edges in both directions (undirected) vs. one direction (directed).
- Use or ignore edge weights (for BFS/DFS, you ignore them; for Dijkstra, you use them if they are nonnegative).


