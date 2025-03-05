# Makefile for CChess
# Written by Renato Fermi <repiazza@gmail.com> in January 2025

UNAME_S := $(shell uname -s)

ifneq ($(findstring _NT-, $(UNAME_S)),)
	_WIN32 = 1
else
	LINUX = 1
endif

# Compiler
CC = gcc

# Paths
SRC_PATH = src
INCLUDE_PATH = include
OBJ_DIR = obj
BIN_DIR = bin

# Compiler options
CCOPT = -Wall -Wextra
INCDIR = -I$(INCLUDE_PATH)

# SDL2 libraries
SDL_LIBS =  -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

# Executable
CCHESS_EXEC = $(BIN_DIR)/cchess

# Object files
#OBJS = \
#    $(OBJ_DIR)/cchess.o \
#    $(OBJ_DIR)/colors.o \
#    $(OBJ_DIR)/trace.o  \
#    $(OBJ_DIR)/event.o  \
#    $(OBJ_DIR)/piece.o  \
#    $(OBJ_DIR)/board.o  \
#		$(OBJ_DIR)/util.o  \
#		$(OBJ_DIR)/cmdline.o  \
#    $(OBJ_DIR)/movement.o
SRC     = $(wildcard $(SRC_PATH)/*.c)
OBJS    = $(patsubst $(SRC_PATH)/%,$(OBJ_DIR)/%,$(SRC:.c=.o))

# Platform-specific settings
ifdef _WIN32
	CCOPT += -D_WIN32
	LIBS = -lmingw32 $(SDL_LIBS) -mwindows
else
	CCOPT += -DLINUX
	LIBS = $(SDL_LIBS)
endif

# Debug flags
DEBUG_ADD_FLAGS = -O2
ifdef DEBUG
	DEBUG_ADD_FLAGS = -g -ggdb
endif

# Default target
all: clean directories $(CCHESS_EXEC)

# Clean target
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) *.log

# Create directories
directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Build executable
$(CCHESS_EXEC): $(OBJS)
	$(CC) $(CCOPT) $(DEBUG_ADD_FLAGS) $(INCDIR) -o $@ $(OBJS) $(LIBS)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_PATH)/%.c
	$(CC) -c $(CCOPT) $(DEBUG_ADD_FLAGS) $(INCDIR) $< -o $@

# Install and uninstall targets (Linux only)
ifdef LINUX
install: all
	@echo "Installing CChess..."
	@sudo cp $(CCHESS_EXEC) /usr/local/bin/cchess
	@echo "CChess installed successfully."

uninstall:
	@echo "Uninstalling CChess..."
	@sudo rm -f /usr/local/bin/cchess
	@echo "CChess uninstalled successfully."
endif

.PHONY: all clean directories install uninstall
