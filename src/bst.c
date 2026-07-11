#include "bst.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// TODO graceful frees when exit

// Return pseudorandom integer in range [0..n)
static int randof(int n) { return rand() / (RAND_MAX + 1.0) * n; }

void init_tree(Tree** t) {
  *t = NULL;
  srand((unsigned)time(NULL));
}

void delete_tree(Tree** t) {
  if (*t) {
    delete_tree(&(*t)->nested);
    delete_tree(&(*t)->left);
    delete_tree(&(*t)->right);

    free((*t)->value);
    free(*t);
    *t = NULL;
  }
}

void print_infix(Tree const* t) {
  if (t) {
    print_infix(t->left);
    puts(t->value);
    print_infix(t->right);
  }
}

Tree const* find_path(Tree** t, char const* const* arr, size_t arr_size,
                      size_t i) {
  return *find_path_pointer(t, arr, arr_size, i);
}

Tree** find_path_pointer(Tree** t, char const* const* arr, size_t arr_size,
                         size_t i) {
  Tree** ans = NULL;
  if ((*t) && (i < arr_size)) {
    int comp_res = strcmp(arr[i], (*t)->value);
    ans = comp_res != 0
              ? find_path_pointer(comp_res < 0 ? &(*t)->left : &(*t)->right,
                                  arr, arr_size, i)
          : i == arr_size - 1
              ? t
              : find_path_pointer(&(*t)->nested, arr, arr_size, i + 1);
  }
  return ans;
}

Tree const* find_path_with_wildcard(Tree* t, char const* const* arr,
                                    size_t arr_size, size_t i) {
  Tree const* ans = NULL;
  if (t && i < arr_size) {
    if (strcmp(arr[i], "*") == 0) {
      ans = find_path_with_wildcard(t->nested, arr, arr_size, i + 1);
      if (!ans) {
        ans = find_path_with_wildcard(t->left, arr, arr_size, i);
        if (!ans) ans = find_path_with_wildcard(t->right, arr, arr_size, i);
      }
    } else {
      int comp_res = strcmp(arr[i], t->value);
      ans = comp_res != 0
                ? find_path_with_wildcard(comp_res < 0 ? t->left : t->right,
                                          arr, arr_size, i)
            : i == arr_size - 1
                ? t
                : find_path_with_wildcard(t->nested, arr, arr_size, i + 1);
    }
  }
  return ans;
}

void push_tree(Tree** t, char const* const* arr, size_t arr_size, size_t i) {
  if (i >= arr_size) return;

  if (*t) {
    int comp_res = strcmp(arr[i], (*t)->value);
    if (comp_res != 0) {
      push_tree(comp_res < 0 ? &(*t)->left : &(*t)->right, arr, arr_size, i);
    } else {
      push_tree(&(*t)->nested, arr, arr_size, i + 1);
    }
  } else {
    *t = malloc(sizeof **t);
    if (*t == NULL) exit(EXIT_FAILURE);

    (*t)->value = malloc((strlen(arr[i]) + 1) * sizeof(*t)->value);
    if ((*t)->value == NULL) exit(EXIT_FAILURE);
    strcpy((*t)->value, arr[i]);

    init_tree(&(*t)->left);
    init_tree(&(*t)->right);
    init_tree(&(*t)->nested);

    push_tree(&(*t)->nested, arr, arr_size, i + 1);
  }
}

// Makes max leaf of left tree the new root.
static void delete_root_max(Tree** t) {
  assert(*t != NULL);
  Tree* temp = *t;

  if ((*t)->left) {
    Tree *maximum = (*t)->left, *parent = *t;

    while (maximum->right) {
      parent = maximum;
      maximum = maximum->right;
    }

    if (parent != *t) {
      parent->right = maximum->left;
      maximum->left = (*t)->left;
    }
    maximum->right = (*t)->right;

    *t = maximum;
  } else {
    *t = (*t)->right;
  }

  delete_tree(&(temp->nested));
  free(temp->value);
  free(temp);
}

// Makes min leaf of right tree the new root.
static void delete_root_min(Tree** t) {
  assert(*t != NULL);
  Tree* temp = *t;

  if ((*t)->right) {
    Tree *minimum = (*t)->right, *parent = *t;

    while (minimum->left) {
      parent = minimum;
      minimum = minimum->left;
    }

    if (parent != *t) {
      parent->left = minimum->right;
      minimum->left = (*t)->left;
    }
    minimum->right = (*t)->right;

    *t = minimum;
  } else {
    *t = (*t)->left;
  }

  delete_tree(&(temp->nested));
  free(temp->value);
  free(temp);
}

void delete_value_from_tree(Tree** t, char const* x) {
  if (*t) {
    int comp_res = strcmp(x, (*t)->value);
    if (comp_res != 0) {
      delete_value_from_tree(comp_res < 0 ? &(*t)->left : &(*t)->right, x);
    } else {
      if (randof(2))
        delete_root_max(t);
      else
        delete_root_min(t);
    }
  }
}
