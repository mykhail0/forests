#include "bst.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

void print_infix(Tree* t) {
  if (t) {
    print_infix(t->left);
    puts(t->value);
    print_infix(t->right);
  }
}

Tree* find_path(Tree* t, char** arr, int arr_size, int i) {
  Tree* ans = NULL;
  if (t && (i < arr_size)) {
    int comp_res = strcmp(arr[i], t->value);
    if (comp_res < 0) {
      ans = find_path(t->left, arr, arr_size, i);
    } else if (comp_res == 0) {
      if (i == arr_size - 1)
        ans = t;
      else
        ans = find_path(t->nested, arr, arr_size, i + 1);
    } else {
      ans = find_path(t->right, arr, arr_size, i);
    }
  }
  return ans;
}

Tree* find_path_with_wildcard(Tree* t, char** arr, int arr_size, int i) {
  Tree* ans = NULL;
  if (t && (i < arr_size)) {
    if (strcmp(arr[i], "*") == 0) {
      ans = find_path_with_wildcard(t->nested, arr, arr_size, i + 1);
      if (!ans) {
        ans = find_path_with_wildcard(t->left, arr, arr_size, i);
        if (!ans) ans = find_path_with_wildcard(t->right, arr, arr_size, i);
      }
    } else {
      int comp_res = strcmp(arr[i], t->value);
      if (comp_res < 0) {
        ans = find_path_with_wildcard(t->left, arr, arr_size, i);
      } else if (comp_res == 0) {
        if (i == arr_size - 1)
          ans = t;
        else
          ans = find_path_with_wildcard(t->nested, arr, arr_size, i + 1);
      } else {
        ans = find_path_with_wildcard(t->right, arr, arr_size, i);
      }
    }
  }
  return ans;
}

Tree** find_path_pointer(Tree** t, char** arr, int arr_size, int i) {
  Tree** ans = NULL;
  if ((*t) && (i < arr_size)) {
    int comp_res = strcmp(arr[i], (*t)->value);
    if (comp_res < 0) {
      ans = find_path_pointer(&(*t)->left, arr, arr_size, i);
    } else if (comp_res == 0) {
      if (i == arr_size - 1)
        ans = t;
      else
        ans = find_path_pointer(&(*t)->nested, arr, arr_size, i + 1);
    } else {
      ans = find_path_pointer(&(*t)->right, arr, arr_size, i);
    }
  }
  return ans;
}

void push_tree(Tree** t, char** arr, int arr_size, int i) {
  if (i >= arr_size) return;

  if (*t) {
    int comp_res = strcmp(arr[i], (*t)->value);
    if (comp_res < 0) {
      push_tree(&(*t)->left, arr, arr_size, i);
    } else if (comp_res == 0) {
      push_tree(&(*t)->nested, arr, arr_size, i + 1);
    } else {
      push_tree(&(*t)->right, arr, arr_size, i);
    }
  } else {
    *t = malloc(sizeof **t);
    if (*t == NULL) exit(EXIT_FAILURE);

    (*t)->value = malloc(strlen(arr[i]) + 1);  // `+ 1` dla '\0'.
    if ((*t)->value == NULL) exit(EXIT_FAILURE);
    strcpy((*t)->value, arr[i]);

    init_tree(&(*t)->left);
    init_tree(&(*t)->right);
    init_tree(&(*t)->nested);

    push_tree(&(*t)->nested, arr, arr_size, i + 1);
  }
}

// NIE DOSTAJA PUSTEGO DRZEWA
// Zamienia korzen drzewa `t` na maksymalny lisc lewego poddrzewa, usuwajac ten
// lisc.
void change_root_to_max_and_delete_max(Tree** t) {
  Tree* temp = *t;

  if ((*t)->left) {
    Tree *maximum = (*t)->left, *parent = *t;

    while (maximum->right) {
      parent = maximum;
      maximum = maximum->right;
    }

    parent->right = maximum->left;
    maximum->left = (*t)->left;
    maximum->right = (*t)->right;

    *t = maximum;
  } else {
    *t = (*t)->right;
  }

  delete_tree(&(temp->nested));
  free(temp->value);
  free(temp);
}

// Zamienia korzen drzewa `t` na minimalny lisc prawego poddrzewa, usuwajac ten
// lisc.
void change_root_to_min_and_delete_min(Tree** t) {
  Tree* temp = *t;

  if ((*t)->right) {
    Tree *minimum = (*t)->right, *parent = *t;

    while (minimum->left) {
      parent = minimum;
      minimum = minimum->left;
    }

    parent->left = minimum->right;
    minimum->left = (*t)->left;
    minimum->right = (*t)->right;

    *t = minimum;
  } else {
    *t = (*t)->left;
  }

  delete_tree(&(temp->nested));
  free(temp->value);
  free(temp);
}

void delete_value_from_tree(Tree** t, char* x) {
  if (*t) {
    int comp_res = strcmp(x, (*t)->value);
    if (comp_res < 0) {
      delete_value_from_tree(&(*t)->left, x);
    } else if (comp_res == 0) {
      if (randof(2))
        change_root_to_max_and_delete_max(t);
      else
        change_root_to_min_and_delete_min(t);
    } else {
      delete_value_from_tree(&(*t)->right, x);
    }
  }
}
