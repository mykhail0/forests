#include <limits.h>
#include <stdlib.h>
#include <time.h>

#include "bst.h"
#include "input.h"

int main() {
  srand((unsigned)time(NULL));
  Tree* forests;
  init_tree(&forests);
  process_commands(&forests);
  delete_tree(&forests);
  return 0;
}
