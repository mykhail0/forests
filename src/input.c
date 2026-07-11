#define _GNU_SOURCE
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"

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

// Assumes `*str` is not empty as a result of `getline()` call.
static void process_line(Tree** forests, char* line) {
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
