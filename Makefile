CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

.PHONY: all clean test

SRCDIR = src
BUILDDIR = build
TESTSDIR = tests
TEST_EXECUTABLE = $(BUILDDIR)/forests

all: forests

$(TEST_EXECUTABLE): CFLAGS += -pedantic -Werror -fstack-protector-strong -g
$(TEST_EXECUTABLE): $(SRCDIR)/forests.c $(BUILDDIR)/bst.o
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $(BUILDDIR)/forest.o
	$(CC) -o $@ $(BUILDDIR)/forest.o $(BUILDDIR)/bst.o

forests: $(SRCDIR)/forests.c $(BUILDDIR)/bst.o
	$(CC) $(CFLAGS) -c $< -o $(BUILDDIR)/forest.o
	$(CC) -o $@ $(BUILDDIR)/forest.o $(BUILDDIR)/bst.o

$(BUILDDIR)/bst.o: $(SRCDIR)/bst.c $(SRCDIR)/bst.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build
	rm forests

test: test.sh $(TEST_EXECUTABLE) $(TESTSDIR)
	./test.sh $(TEST_EXECUTABLE) $(TESTSDIR)
