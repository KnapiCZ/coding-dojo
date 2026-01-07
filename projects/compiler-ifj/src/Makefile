# === Makefile for your C project ===
# Compiles all .c files in the current directory into one executable called "compiler"

# Compiler and flags

#
# use sanitize for another thing like valgrind, cannot be used simulatneousy (idk spelling)
#


CC := gcc
CFLAGS := -Wall -Wextra -Werror -std=c99 #-fsanitize=address

# Source files and object files
SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)

# Output binary name
TARGET := compiler

# === Default rule ===
all: $(TARGET)

# === Compile each .c file into .o ===
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# === Link all object files into the final binary ===
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# === Run the compiled program ===
run: $(TARGET)
	./$(TARGET)

# === Clean build artifacts ===
clean:
	rm -f $(OBJ) $(TARGET)

# === Phony targets (not actual files) ===
.PHONY: all run clean
