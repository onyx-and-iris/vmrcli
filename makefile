program = vmrcli

CC = gcc
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

EXE := $(BIN_DIR)/$(program).exe
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

LOG_USE_COLOR ?= yes
ifeq ($(LOG_USE_COLOR), yes)
	CPPFLAGS := -Iinclude -MMD -MP -DLOG_USE_COLOR
else
	CPPFLAGS := -Iinclude -MMD -MP
endif
CFLAGS = -O -Wall -W -pedantic -ansi -std=c2x
LDFLAGS  := -Llib
LDLIBS   := -lm

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	pwsh -Command New-Item -Path $@ -ItemType Directory

clean:
	pwsh -Command Remove-Item -Recurse $(EXE), $(OBJ_DIR) -force

-include $(OBJ:.o=.d)
