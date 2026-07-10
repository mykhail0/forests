typedef struct tree Tree;

struct tree {
  char* value;
  Tree* nested;
  Tree *left, *right;
};

// Make an empty tree.
void init_tree(Tree**);

void delete_tree(Tree**);

void print_infix(Tree* t);

// Follows the given array of strings to go down the path in the given tree
// recursively. Returns the found node.
Tree* find_path(Tree*, char**, int, int);

// Same as `find_path` but with support for wildcard `*`.
Tree* find_path_with_wildcard(Tree* t, char** arr, int arr_size, int i);

// Same as `find_path` but returns a pointer to the found node.
Tree** find_path_pointer(Tree** t, char** arr, int arr_size, int i);

/**
 * Wstawia do drzewa `*t` ciag napisow `*arr` o `arr_size` slowach.
 * Kolejne slowo jest w kolejnym "zaglebieniu" (pole `derivative`).
 * `i` to pozycja w tablicy `arr` klucza,
 * ktory jest wartoscia aktualnie tworzonego wezla w `t`.
 */
void push_tree(Tree** t, char** arr, int arr_size, int i);

void delete_value_from_tree(Tree** t, char* x);
