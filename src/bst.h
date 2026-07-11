#include <stdlib.h>

// User of this library must initiate pseudorandom numbers with:
// srand((unsigned)time(NULL));
// for proper balancing of BST.

typedef struct tree Tree;

struct tree {
  char* value;
  Tree* nested;
  Tree *left, *right;
};

// Make an empty tree.
void init_tree(Tree**);

void delete_tree(Tree**);

void print_infix(Tree const*);

// Follows the given array of strings to go down the path in the given tree
// recursively. Returns the found node.
Tree const* find_path(Tree**, char const* const*, size_t, size_t);

// Same as `find_path` but with support for wildcard `*`.
Tree const* find_path_with_wildcard(Tree*, char const* const*, size_t, size_t);

// Same as `find_path` but returns a pointer to the found node.
Tree** find_path_pointer(Tree**, char const* const*, size_t, size_t);

/**
 * Inserts a given sequence of the given length of strings into the given tree
 * recursively, each next word nesting deeper into the tree.
 */
void push_tree(Tree**, char const* const*, size_t, size_t);

void delete_value_from_tree(Tree**, char const*);
