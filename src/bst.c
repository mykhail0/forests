#include "bst.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Return pseudorandom integer in range [0..n)
static int randof(int n) { return rand() / (RAND_MAX + 1.0) * n; }

void init_tree(Tree** tp) { *tp = NULL; }

void delete_tree(Tree** tp) {
  if (*tp) {
    delete_tree(&(*tp)->nested);
    delete_tree(&(*tp)->left);
    delete_tree(&(*tp)->right);

    free((*tp)->value);
    free(*tp);
    *tp = NULL;
  }
}

void print_infix(Tree const* t) {
  if (t) {
    print_infix(t->left);
    puts(t->value);
    print_infix(t->right);
  }
}

Tree const* find_path(Tree** tp, char const* const* arr, size_t arr_size,
                      size_t i) {
  Tree** ret_tp = find_path_pointer(tp, arr, arr_size, i);
  return ret_tp == NULL ? NULL : *ret_tp;
}

Tree** find_path_pointer(Tree** tp, char const* const* arr, size_t arr_size,
                         size_t i) {
  Tree** ans = NULL;
  if ((*tp) && (i < arr_size)) {
    int comp_res = strcmp(arr[i], (*tp)->value);
    ans = comp_res != 0
              ? find_path_pointer(comp_res < 0 ? &(*tp)->left : &(*tp)->right,
                                  arr, arr_size, i)
          : i == arr_size - 1
              ? tp
              : find_path_pointer(&(*tp)->nested, arr, arr_size, i + 1);
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

bool push_tree(Tree** tp, char const* const* arr, size_t arr_size, size_t i) {
  if (i >= arr_size) return true;

  if (*tp) {
    int comp_res = strcmp(arr[i], (*tp)->value);
    if (comp_res != 0) {
      return push_tree(comp_res < 0 ? &(*tp)->left : &(*tp)->right, arr,
                       arr_size, i);
    } else {
      return push_tree(&(*tp)->nested, arr, arr_size, i + 1);
    }
  } else {
    *tp = malloc(sizeof **tp);
    if (*tp == NULL) return false;

    init_tree(&(*tp)->left);
    init_tree(&(*tp)->right);
    init_tree(&(*tp)->nested);

    (*tp)->value = calloc(strlen(arr[i]) + 1, sizeof(*tp)->value);
    // if ((*tp)->value == NULL) return false;
    strcpy((*tp)->value, arr[i]);

    return push_tree(&(*tp)->nested, arr, arr_size, i + 1);
  }
}

// Makes max leaf of left tree the new root.
static void delete_root_max(Tree** tp) {
  assert(*tp != NULL);
  assert((*tp)->left != NULL);
  Tree *temp = *tp, *maximum = (*tp)->left, *parent = *tp;

  while (maximum->right) {
    parent = maximum;
    maximum = maximum->right;
  }

  if (parent != *tp) {
    parent->right = maximum->left;
    maximum->left = (*tp)->left;
  }
  maximum->right = (*tp)->right;

  *tp = maximum;

  delete_tree(&temp->nested);
  free(temp->value);
  free(temp);
}

// Makes min leaf of right tree the new root.
static void delete_root_min(Tree** tp) {
  assert(*tp != NULL);
  assert((*tp)->right != NULL);
  Tree *temp = *tp, *minimum = (*tp)->right, *parent = *tp;

  while (minimum->left) {
    parent = minimum;
    minimum = minimum->left;
  }

  if (parent != *tp) {
    parent->left = minimum->right;
    minimum->right = (*tp)->right;
  }
  minimum->left = (*tp)->left;

  *tp = minimum;

  delete_tree(&temp->nested);
  free(temp->value);
  free(temp);
}

void delete_value_from_tree(Tree** tp, char const* x) {
  if (*tp) {
    int comp_res = strcmp(x, (*tp)->value);
    if (comp_res != 0) {
      delete_value_from_tree(comp_res < 0 ? &(*tp)->left : &(*tp)->right, x);
    } else {
      // If there exists only one child then make it root. Otherwise, at random
      // make the root either max from left child or min from right child, this
      // makes BST balanced on average.
      if ((*tp)->left == NULL || (*tp)->right == NULL) {
        delete_tree(&(*tp)->nested);
        free((*tp)->value);
        Tree* temp = *tp;
        *tp = (*tp)->left == NULL ? (*tp)->right : (*tp)->left;
        free(temp);
      } else if (randof(2)) {
        delete_root_max(tp);
      } else {
        delete_root_min(tp);
      }
    }
  }
}
