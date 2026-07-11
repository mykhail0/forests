#define _GNU_SOURCE
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "bst.h"

/**
 * Find starts of each word in command `str` and write them into an array. If
 * the characters in this or number of words are not valid, returns 1, else
 * returns 0.
 */
static int split_command(char* str, char* arr[], size_t* arr_size) {
  static const char whitespace[7] = {' ', '\t', '\n', '\v', '\f', '\r', '\0'};
  bool ended_with_newline = str[strlen(str) - 1] == '\n';
  for (size_t i = 0; i < *arr_size; ++i) arr[i] = NULL;

  size_t i = 0;
  char* word = strtok(str, whitespace);

  while (word != NULL && i < *arr_size) {
    size_t len = strlen(word);
    for (size_t j = 0; j < len; ++j) {
      if ((unsigned char)word[j] < 33) return EXIT_FAILURE;
    }
    arr[i] = word;

    word = strtok(NULL, whitespace);
    ++i;
  }

  *arr_size = i;

  return word != NULL                             ? EXIT_FAILURE
         : (*arr_size == 0 || ended_with_newline) ? EXIT_SUCCESS
                                                  : EXIT_FAILURE;
}

void handle_ADD(Tree** forests, char const* const* command,
                size_t command_size) {
  if (command_size == 1) {
    fprintf(stderr, "ERROR\n");
    return;
  }

  push_tree(forests, command, command_size, 1);
  puts("OK");
}

void handle_DEL(Tree** forests, char const* const* command,
                size_t command_size) {
  if (command_size == 1) {
    delete_tree(forests);
  } else {
    Tree** tree_to_delete =
        find_path_pointer(forests, command, command_size, 1);
    if (tree_to_delete)
      delete_value_from_tree(tree_to_delete, command[command_size - 1]);
  }
  puts("OK");
}

void handle_PRINT(Tree** forests, char const* const* command,
                  size_t command_size) {
  if (command_size == 4) {
    fprintf(stderr, "ERROR\n");
    return;
  }

  if (command_size == 1) {
    print_infix(*forests);
  } else {
    Tree const* tree_to_print = find_path(forests, command, command_size, 1);
    if (tree_to_print) print_infix(tree_to_print->nested);
  }
}

void handle_CHECK(Tree* forests, char const* const* command,
                  size_t command_size) {
  if (command_size == 1 || strcmp(command[command_size - 1], "*") == 0)
    fprintf(stderr, "ERROR\n");
  else
    puts(find_path_with_wildcard(forests, command, command_size, 1) ? "YES"
                                                                    : "NO");
}

void handle_command(Tree** forests, char const* const* const command,
                    size_t command_size) {
  if (command_size == 0) {
    return;
  }
  if (strcmp(command[0], "ADD") == 0) {
    handle_ADD(forests, command, command_size);
  } else if (strcmp(command[0], "DEL") == 0) {
    handle_DEL(forests, command, command_size);
  } else if (strcmp(command[0], "PRINT") == 0) {
    handle_PRINT(forests, command, command_size);
  } else if (strcmp(command[0], "CHECK") == 0) {
    handle_CHECK(*forests, command, command_size);
  } else {
    fprintf(stderr, "ERROR\n");
  }
}

// Assumes `*str` is not empty as a result of `getline()` call.
void process_line(Tree** forests, char* line) {
  if (line[0] != '#') {
    // Maximum length of the command.
    size_t command_size = 4;
    char* command[4];

    if (split_command(line, command, &command_size) == EXIT_SUCCESS)
      handle_command(forests, (char const* const*)command, command_size);
    else
      fprintf(stderr, "ERROR\n");
  }
}

void process_commands(Tree** forests) {
  char* line = NULL;
  size_t line_size = 0;
  while (getline(&line, &line_size, stdin) > 0) process_line(forests, line);
  int errsv = errno;
  free(line);

  if (ferror(stdin) || errsv == EINVAL || errsv == ENOMEM) {
    delete_tree(forests);
    perror("getline()");
    exit(EXIT_FAILURE);
  }
}

int main() {
  srand((unsigned)time(NULL));
  Tree* forests;
  init_tree(&forests);
  process_commands(&forests);
  delete_tree(&forests);
  return 0;
}
