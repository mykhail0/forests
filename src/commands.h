#ifndef COMMANDS_H
#define COMMANDS_H

#include "bst.h"

void printERR();

// Given an array of strings with its size, which contains a command and the
// world's database, handle the command. Return `false` iff memory allocation
// error.
bool handle_command(Tree**, char const* const* const, size_t);

#endif  // COMMANDS_H
