CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

.PHONY: all clean test

SRCDIR = src
BUILDDIR = build
TESTSDIR = tests
TEST_EXECUTABLE = $(BUILDDIR)/forests

all: forests

$(TEST_EXECUTABLE): CFLAGS += -pedantic -Werror -fstack-protector-strong -g
$(TEST_EXECUTABLE): $(SRCDIR)/forests.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $< -o $@

forests: $(SRCDIR)/forests.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf build

test: test.sh $(TEST_EXECUTABLE) $(TESTSDIR)
	./test.sh $(TEST_EXECUTABLE) $(TESTSDIR)
