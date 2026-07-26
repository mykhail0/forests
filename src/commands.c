#include <stdio.h>
#include <string.h>

#include "bst.h"

void printERR() { fprintf(stderr, "ERROR\n"); }

// Return `false` when memory allocation failed.
static bool handle_ADD(Tree** forests, char const* const* command,
                       size_t command_size) {
  if (command_size == 1) {
    printERR();
    return true;
  }

  if (!push_tree(forests, command, command_size, 1)) {
    printERR();
    return false;
  }
  puts("OK");
  return true;
}

static void handle_DEL(Tree** forests, char const* const* command,
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

static void handle_PRINT(Tree** forests, char const* const* command,
                         size_t command_size) {
  if (command_size == 4) {
    printERR();
    return;
  }

  if (command_size == 1) {
    print_infix(*forests);
  } else {
    Tree const* tree_to_print = find_path(forests, command, command_size, 1);
    if (tree_to_print) print_infix(tree_to_print->nested);
  }
}

static void handle_CHECK(Tree* forests, char const* const* command,
                         size_t command_size) {
  if (command_size == 1 || strcmp(command[command_size - 1], "*") == 0)
    printERR();
  else
    puts(find_path_with_wildcard(forests, command, command_size, 1) ? "YES"
                                                                    : "NO");
}

bool handle_command(Tree** forests, char const* const* const command,
                    size_t command_size) {
  if (command_size > 0) {
    if (strcmp(command[0], "ADD") == 0) {
      return handle_ADD(forests, command, command_size);
    } else if (strcmp(command[0], "DEL") == 0) {
      handle_DEL(forests, command, command_size);
    } else if (strcmp(command[0], "PRINT") == 0) {
      handle_PRINT(forests, command, command_size);
    } else if (strcmp(command[0], "CHECK") == 0) {
      handle_CHECK(*forests, command, command_size);
    } else {
      printERR();
    }
  }
  return true;
}
