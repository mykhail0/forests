#include <limits.h>
#include <stdlib.h>
#include <time.h>

#include "bst.h"
#include "input.h"

int main() {
  srand((unsigned)time(NULL));
  Tree* forests;
  init_tree(&forests);
  int exit_code = process_commands(&forests) ? EXIT_SUCCESS : EXIT_FAILURE;
  delete_tree(&forests);
  return exit_code;
}
