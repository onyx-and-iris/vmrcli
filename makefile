# Program name
program = vmrcli

# Compiler
CC = gcc

# Directories
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

# Executable and source/object files
EXE := $(BIN_DIR)/$(program).exe
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Conditional compilation flags for logging
LOG_USE_COLOR ?= yes
ifeq ($(LOG_USE_COLOR), yes)
	CPPFLAGS := -Iinclude -MMD -MP -DLOG_USE_COLOR
else
	CPPFLAGS := -Iinclude -MMD -MP
endif

# Compiler and linker flags
CFLAGS = -O -Wall -W -pedantic -ansi -std=c2x
LDFLAGS  := -Llib
LDLIBS   := -lm

# Phony targets
.PHONY: all clean

# Default target
all: $(EXE)

# Link the executable
$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Create necessary directories
$(BIN_DIR) $(OBJ_DIR):
	pwsh -Command New-Item -Path $@ -ItemType Directory

# Clean up generated files
clean:
	pwsh -Command Remove-Item -Recurse $(BIN_DIR), $(OBJ_DIR) -force

# Include dependency files
-include $(OBJ:.o=.d)
