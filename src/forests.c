#define _GNU_SOURCE
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SMALLER (-1)
#define EQUAL 0
#define BIGGER 1
#define MAX_PATH_LENGTH 3
#define MAX_PRINT_ARGUMENTS_NUMBER 2
#define MIN_ADD_ARGUMENTS_NUMBER 1
#define MIN_CHECK_ARGUMENTS_NUMBER 1

typedef struct tree Tree;

struct tree {
  char* value;
  Tree* derivative;
  Tree *left, *right;
};

// Return pseudorandom integer in range [0..n)
int randof(int n) { return rand() / (RAND_MAX + 1.0) * n; }

// Make an empty tree.
void init_tree(Tree** t) { *t = NULL; }

void delete_tree(Tree** t) {
  if (*t) {
    delete_tree(&(*t)->derivative);
    delete_tree(&(*t)->left);
    delete_tree(&(*t)->right);

    free((*t)->value);
    free(*t);
    *t = NULL;
  }
}

// Wypisuje wartosci drzewa w przejsciu infiksowym.
void print_infix(Tree* t) {
  if (t) {
    print_infix(t->left);
    puts(t->value);
    print_infix(t->right);
  }
}

/**
 * Pomocnicza funkcja do obslugi `push_tree()` i `delete_value_form_tree()`.
 * Zwraca BIGGER, jesli	`a` jest wieksze leksykograficznie od `b`,
 * SMALLER, jesli odwrotnie,
 * EQUAL, jesli `a` = `b`.
 * `strcmp()` ale z ustalonymi `return value`.
 */
int compare_strings(char* a, char* b) {
  int compareCode = strcmp(a, b);
  if (compareCode > 0) {
    compareCode = BIGGER;
  } else {
    if (compareCode < 0) compareCode = SMALLER;
  }
  return compareCode;
}

// BST
/**
 * Poszukuje w drzewie `t` ciagu napisow `*arr` o `arr_size` slowach.
 * `i` to pozycja aktualnie poszukiwanego klucza w tablicy `arr`.
 * Funkcja dla `handlePRINT()`.
 */
Tree* find_path_tail(Tree* t, char** arr, int arr_size, int i) {
  Tree* ans = NULL;
  if (t && (i < arr_size)) {
    int compareCode = compare_strings(arr[i], t->value);
    switch (compareCode) {
      case SMALLER:
        ans = find_path_tail(t->left, arr, arr_size, i);
        break;
      case EQUAL:
        if (i == arr_size - 1)
          ans = t;
        else
          ans = find_path_tail(t->derivative, arr, arr_size, i + 1);
        break;
      case BIGGER:
        ans = find_path_tail(t->right, arr, arr_size, i);
        break;
    }
  }
  return ans;
}

/**
 * To samo co `find_path_tail()` ale z metaznakiem `*`.
 * Funkcja dla `handleCHECK()`.
 */
Tree* find_path_tail_with_metasign(Tree* t, char** arr, int arr_size, int i) {
  Tree* ans = NULL;
  if (t && (i < arr_size)) {
    if (compare_strings(arr[i], "*") == EQUAL) {
      ans = find_path_tail_with_metasign(t->derivative, arr, arr_size, i + 1);
      if (!ans) {
        ans = find_path_tail_with_metasign(t->left, arr, arr_size, i);
        if (!ans)
          ans = find_path_tail_with_metasign(t->right, arr, arr_size, i);
      }
    } else {
      int compareCode = compare_strings(arr[i], t->value);
      switch (compareCode) {
        case SMALLER:
          ans = find_path_tail_with_metasign(t->left, arr, arr_size, i);
          break;
        case EQUAL:
          if (i == arr_size - 1)
            ans = t;
          else
            ans = find_path_tail_with_metasign(t->derivative, arr, arr_size,
                                               i + 1);
          break;
        case BIGGER:
          ans = find_path_tail_with_metasign(t->right, arr, arr_size, i);
          break;
      }
    }
  }
  return ans;
}

/**
 * To samo co `find_path_tail()` ale zwraca wskaznik na wskaznik na lisc,
 * co umozliwia jego zmiane.
 * Funkcja dla `handleDEL()`.
 */
Tree** find_path_tail_pointer(Tree** t, char** arr, int arr_size, int i) {
  Tree** ans = NULL;
  if ((*t) && (i < arr_size)) {
    int compareCode = compare_strings(arr[i], (*t)->value);
    switch (compareCode) {
      case SMALLER:
        ans = find_path_tail_pointer(&(*t)->left, arr, arr_size, i);
        break;
      case EQUAL:
        if (i == arr_size - 1)
          ans = t;
        else
          ans = find_path_tail_pointer(&(*t)->derivative, arr, arr_size, i + 1);
        break;
      case BIGGER:
        ans = find_path_tail_pointer(&(*t)->right, arr, arr_size, i);
        break;
    }
  }
  return ans;
}

/**
 * Wstawia do drzewa `*t` ciag napisow `*arr` o `arr_size` slowach.
 * Kolejne slowo jest w kolejnym "zaglebieniu" (pole `derivative`).
 * `i` to pozycja w tablicy `arr` klucza,
 * ktory jest wartoscia aktualnie tworzonego wezla w `t`.
 */
void push_tree(Tree** t, char** arr, int arr_size, int i) {
  if (i >= arr_size) return;

  if (*t) {
    int compareCode = compare_strings(arr[i], (*t)->value);
    switch (compareCode) {
      case SMALLER:
        push_tree(&(*t)->left, arr, arr_size, i);
        break;
      case EQUAL:
        push_tree(&(*t)->derivative, arr, arr_size, i + 1);
        break;
      case BIGGER:
        push_tree(&(*t)->right, arr, arr_size, i);
        break;
    }
  } else {
    *t = malloc(sizeof **t);
    if (*t == NULL) exit(EXIT_FAILURE);

    (*t)->value = malloc(strlen(arr[i]) + 1);  // `+ 1` dla '\0'.
    if ((*t)->value == NULL) exit(EXIT_FAILURE);
    strcpy((*t)->value, arr[i]);

    init_tree(&(*t)->left);
    init_tree(&(*t)->right);
    init_tree(&(*t)->derivative);

    push_tree(&(*t)->derivative, arr, arr_size, i + 1);
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

  delete_tree(&(temp->derivative));
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

  delete_tree(&(temp->derivative));
  free(temp->value);
  free(temp);
}

// Usuwa lisc o wartosci `value` z drzewa `t`.
void delete_value_from_tree(Tree** t, char* x) {
  if (*t) {
    int compareCode = compare_strings(x, (*t)->value);
    switch (compareCode) {
      case SMALLER:
        delete_value_from_tree(&(*t)->left, x);
        break;
      case EQUAL:
        if (randof(2))
          change_root_to_max_and_delete_max(t);
        else
          change_root_to_min_and_delete_min(t);
        break;
      case BIGGER:
        delete_value_from_tree(&(*t)->right, x);
        break;
    }
  }
}

// Definicja bialych znakow na potrzeby `convert_string_to_arr()`.
const char whiteSpaces[7] = {' ', '\t', '\n', '\v', '\f', '\r', '\0'};

// Czysci tablice stringow `arr` o rozmiarze `arr_size`.
void clear_arr(char** arr, int arr_size) {
  for (int i = 0; i < arr_size; ++i) free(arr[i]);
  free(arr);
}

/**
 * Przepisuje najwyzej `MAX_PATH_LENGTH + 1` slow ze stringa `*str` do tablicy
 * `**arr` o rozmiarze `arr_size`.
 * Zwraca 0, jesli string mial co najwyzej `MAX_PATH_LENGTH + 1` slow
 * i 1, jesli mial wiecej.
 * Funkcja rozpoznaje przerwy pomiedzy slowami za pomoca tablicy `whiteSpaces`.
 */
int convert_string_to_arr(char** str, char*** arr, int* arr_size) {
  *arr_size = MAX_PATH_LENGTH + 1;
  *arr = malloc((*arr_size) * sizeof **arr);
  if (*arr == NULL) exit(EXIT_FAILURE);

  int i = 0;
  char* word = strtok(*str, whiteSpaces);

  while (word != NULL && i < MAX_PATH_LENGTH + 1) {
    // `+ 1` - plus pierwsze slowo-polecenie
    (*arr)[i] = malloc((strlen(word) + 1) * sizeof *((*arr)[i]));
    if ((*arr)[i] == NULL) exit(EXIT_FAILURE);

    strcpy((*arr)[i], word);
    word = strtok(NULL, whiteSpaces);
    i++;
  }

  *arr_size = i;
  *arr = realloc(*arr, (*arr_size) * sizeof **arr);
  if (*arr == NULL) exit(EXIT_FAILURE);

  int ans;
  if (word == NULL)
    ans = EXIT_SUCCESS;
  else
    ans = EXIT_FAILURE;
  return ans;
}

// Obsluga polecenia `ADD`.
void handleADD(Tree** forests, char** arr, int arr_size) {
  if (arr_size < MIN_ADD_ARGUMENTS_NUMBER + 1) {
    fprintf(stderr, "ERROR\n");
    return;
  }

  push_tree(forests, arr, arr_size, 1);
  puts("OK");
}

// Obsluga polecenia `DEL`.
void handleDEL(Tree** forests, char** arr, int arr_size) {
  if (arr_size == 1) {
    delete_tree(forests);
  } else {
    Tree** entity_to_be_deleted =
        find_path_tail_pointer(forests, arr, arr_size, 1);
    if (entity_to_be_deleted)
      delete_value_from_tree(entity_to_be_deleted, arr[arr_size - 1]);
  }
  puts("OK");
}

// Obsluga polecenia `PRINT`.
void handlePRINT(Tree* forests, char** arr, int arr_size) {
  if (arr_size > MAX_PRINT_ARGUMENTS_NUMBER + 1) {
    fprintf(stderr, "ERROR\n");
    return;
  }

  if (arr_size == 1) {
    print_infix(forests);
  } else {
    Tree* entity_to_be_printed = find_path_tail(forests, arr, arr_size, 1);
    if (entity_to_be_printed) print_infix(entity_to_be_printed->derivative);
  }
}

// Obsluga polecenia `CHECK`. NIE OBSLUGUJE POSZUKIWANIA Z METAZNAKIEM
void handleCHECK(Tree* forests, char** arr, int arr_size) {
  if ((arr_size < MIN_CHECK_ARGUMENTS_NUMBER + 1) ||
      (compare_strings(arr[arr_size - 1], "*") == EQUAL)) {
    fprintf(stderr, "ERROR\n");
    return;
  }

  if (find_path_tail_with_metasign(forests, arr, arr_size, 1))
    puts("YES");
  else
    puts("NO");
}

// Obsluga polecenia w linijce z tablicy `**arr`.
void process_arr(Tree** forests, char** arr, int arr_size) {
  if (*arr == NULL) return;
  if (compare_strings(arr[0], "ADD") == EQUAL) {
    handleADD(forests, arr, arr_size);
    return;
  }
  if (compare_strings(arr[0], "DEL") == EQUAL) {
    handleDEL(forests, arr, arr_size);
    return;
  }
  if (compare_strings(arr[0], "PRINT") == EQUAL) {
    handlePRINT(*forests, arr, arr_size);
    return;
  }
  if (compare_strings(arr[0], "CHECK") == EQUAL) {
    handleCHECK(*forests, arr, arr_size);
    return;
  }
  fprintf(stderr, "ERROR\n");
}

// Obsluga linijki.
void process_line(Tree** forests, char** str) {
  if ((*str)[0] != '#') {
    // `*str` nie jest puste bo zostalo wczytane za pomoca `getline()`.
    char** arr;
    int arr_size;

    if (convert_string_to_arr(str, &arr, &arr_size) == EXIT_SUCCESS)
      process_arr(forests, arr, arr_size);
    else
      fprintf(stderr, "ERROR\n");

    clear_arr(arr, arr_size);
  }
}

void interpret_input(Tree** forests) {
  char* str = NULL;
  size_t str_size = 0;
  int errsv = getline(&str, &str_size, stdin);

  while (errsv != -1) {  // Nie wiem, czy to poprawny dozor petli.
    process_line(forests, &str);
    errsv = getline(&str, &str_size, stdin);
  }
  free(str);
}

// Ustawia poczatkowy stan i odpala program.
void the_world_app() {
  // W drzewie `forests` wszystko jest przechowywane.
  Tree* forests;
  srand((unsigned)time(NULL));
  init_tree(&forests);
  interpret_input(&forests);
  delete_tree(&forests);
}

int main() {
  the_world_app();
  return 0;
}
