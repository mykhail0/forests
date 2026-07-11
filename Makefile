CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

.PHONY: all clean test

SRCDIR = src
BUILDDIR = build
TESTSDIR = tests
TEST_EXECUTABLE = $(BUILDDIR)/forests
AUXOBJS = $(BUILDDIR)/input.o $(BUILDDIR)/commands.o $(BUILDDIR)/bst.o

all: forests

$(TEST_EXECUTABLE): CFLAGS += -pedantic -Werror -fstack-protector-strong -g
$(TEST_EXECUTABLE): $(SRCDIR)/forests.c $(AUXOBJS)
	$(CC) $(CFLAGS) -c $< -o $(BUILDDIR)/forest.o
	$(CC) -o $@ $(BUILDDIR)/forest.o $(AUXOBJS)

forests: $(SRCDIR)/forests.c $(AUXOBJS)
	$(CC) $(CFLAGS) -c $< -o $(BUILDDIR)/forest.o
	$(CC) -o $@ $(BUILDDIR)/forest.o $(AUXOBJS)

$(BUILDDIR)/input.o: $(SRCDIR)/input.c $(SRCDIR)/input.h
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/commands.o: $(SRCDIR)/commands.c $(SRCDIR)/commands.h
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/bst.o: $(SRCDIR)/bst.c $(SRCDIR)/bst.h
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	if [ -d "$(BUILDDIR)" ]; then rm -rf "$(BUILDDIR)"; fi
	if [ -f forests ]; then rm forests; fi

test: test.sh $(TEST_EXECUTABLE) $(TESTSDIR)
	./test.sh $(TEST_EXECUTABLE) $(TESTSDIR)
