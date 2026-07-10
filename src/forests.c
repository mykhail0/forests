#define _GNU_SOURCE
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bst.h"

void free_arr(char** arr, int arr_size) {
  for (int i = 0; i < arr_size; ++i) free(arr[i]);
  free(arr);
}

/**
 * Splits a string `str` into words on white spaces and writes them into an
 * array. If there are more words than possible in a valid command then returns
 * 1, else returns 0.
 */
static int split(char* str, char*** arr, int* arr_size) {
  static const char whitespace[7] = {' ', '\t', '\n', '\v', '\f', '\r', '\0'};
  // Maximum length of the command.
  *arr_size = 4;
  *arr = malloc((*arr_size) * sizeof **arr);
  if (*arr == NULL) {
    free(str);
    exit(EXIT_FAILURE);
  }

  int i = 0;
  char* word = strtok(str, whitespace);

  while (word != NULL && i < *arr_size) {
    (*arr)[i] = malloc((strlen(word) + 1) * sizeof *((*arr)[i]));
    if ((*arr)[i] == NULL) {
      free_arr(*arr, i);
      free(str);
      exit(EXIT_FAILURE);
    }
    strcpy((*arr)[i], word);

    word = strtok(NULL, whitespace);
    ++i;
  }

  *arr_size = i;

  return word == NULL ? EXIT_SUCCESS : EXIT_FAILURE;
}

void handle_ADD(Tree** forests, char** command, int command_size) {
  if (command_size == 1) {
    fprintf(stderr, "ERROR\n");
    return;
  }

  push_tree(forests, command, command_size, 1);
  puts("OK");
}

void handle_DEL(Tree** forests, char** command, int command_size) {
  if (command_size == 1) {
    delete_tree(forests);
  } else {
    Tree** entity_to_be_deleted =
        find_path_pointer(forests, command, command_size, 1);
    if (entity_to_be_deleted)
      delete_value_from_tree(entity_to_be_deleted, command[command_size - 1]);
  }
  puts("OK");
}

void handle_PRINT(Tree* forests, char** arr, int command_size) {
  if (command_size == 4) {
    fprintf(stderr, "ERROR\n");
    return;
  }

  if (command_size == 1) {
    print_infix(forests);
  } else {
    Tree* entity_to_be_printed = find_path(forests, arr, command_size, 1);
    if (entity_to_be_printed) print_infix(entity_to_be_printed->nested);
  }
}

// TODO
// NIE OBSLUGUJE POSZUKIWANIA Z METAZNAKIEM
void handle_CHECK(Tree* forests, char** command, int command_size) {
  if (command_size == 1) {
    fprintf(stderr, "ERROR\n");
    return;
  }

  puts(find_path_with_wildcard(forests, command, command_size, 1) ? "YES"
                                                                  : "NO");
}

void handle_command(Tree** forests, char** command, int command_size) {
  assert(*command != NULL);
  if (command_size == 0) {
    return;
  }
  if (strcmp(command[0], "ADD") == 0) {
    handle_ADD(forests, command, command_size);
  } else if (strcmp(command[0], "DEL") == 0) {
    handle_DEL(forests, command, command_size);
  } else if (strcmp(command[0], "PRINT") == 0) {
    handle_PRINT(*forests, command, command_size);
  } else if (strcmp(command[0], "CHECK") == 0) {
    handle_CHECK(*forests, command, command_size);
  } else {
    fprintf(stderr, "ERROR\n");
  }
}

// Assumes `*str` is not empty as a result of `getline()` call.
void process_line(Tree** forests, char* line) {
  if (line[0] != '#') {
    char** command;
    int command_size;

    if (split(line, &command, &command_size) == EXIT_SUCCESS)
      handle_command(forests, command, command_size);
    else
      fprintf(stderr, "ERROR\n");

    free_arr(command, command_size);
  }
}

void process_commands(Tree** forests) {
  char* line = NULL;
  size_t line_size = 0;
  int errsv;

  while (0 < (errsv = getline(&line, &line_size, stdin)))
    process_line(forests, line);

  free(line);

  if (!feof(stdin) && ferror(stdin)) {
    delete_tree(forests);
    perror("getline()");
    exit(EXIT_FAILURE);
  }
}

int main() {
  Tree* forests;
  init_tree(&forests);
  process_commands(&forests);
  delete_tree(&forests);
  return 0;
}
