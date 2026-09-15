CC ?= gcc
CFLAGS = -Wall -Wextra -std=c99 -Isrc

SRCS = src/item.c \
       src/storage.c \
       src/graph.c \
       src/hash_index.c \
       src/ranking.c \
       src/text_match.c \
       src/matcher.c \
       src/registration.c \
       src/cli.c

MAIN_SRC = src/main.c
TEST_SRC = tests/test_runner.c

TARGET = assettrace.exe
TEST_TARGET = test_runner.exe

.PHONY: all clean test run

all: $(TARGET)

$(TARGET): $(MAIN_SRC) $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC) $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET)

clean:
	@if exist $(TARGET) del /Q $(TARGET)
	@if exist $(TEST_TARGET) del /Q $(TEST_TARGET)
	@if exist *.o del /Q *.o
