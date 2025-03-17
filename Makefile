# Makefile for CChess
# Written by Renato Fermi <repiazza@gmail.com> in January 2025

# Compiler
CC = gcc

# Paths
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Compiler options
CCOPT = -Wall -Wextra
CFLAGS = -I$(INCDIR)
LDFLAGS = 

# SDL2 libraries
SDL_LIBS =  -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

# Executable
CCHESS_EXEC = $(BINDIR)/cchess

# Object files
SRC     = $(wildcard $(SRCDIR)/*.c)
OBJS    = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRC:.c=.o))

# Platform-specific settings
ifdef _WIN32
	CCOPT += -D_WIN32
	LIBS = -lmingw32 $(SDL_LIBS) -mwindows
endif

ifdef LINUX
	CCOPT += -DLINUX
	LIBS = $(SDL_LIBS)
endif

ifdef APPLE
	CCOPT += -Wno-main -DAPPLE
	LIBS = $(SDL_LIBS)
	CFLAGS += -I/opt/homebrew/include
	LDFLAGS +=  -L /opt/homebrew/lib
endif


# Debug flags
DEBUG_ADD_FLAGS = -O2
ifdef DEBUG
  ifdef APPLE
    DEBUG_ADD_FLAGS = -g
  else
    DEBUG_ADD_FLAGS = -g -ggdb
  endif
endif

# Default target
all: clean directories $(CCHESS_EXEC)

# Clean target
clean:
	rm -rf $(OBJDIR) $(BINDIR) *.log

# Create directories
directories:
	mkdir -p $(OBJDIR) $(BINDIR)

# Build executable
$(CCHESS_EXEC): $(OBJS)
	$(CC) $(CCOPT) $(DEBUG_ADD_FLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CCOPT) $(DEBUG_ADD_FLAGS) $(CFLAGS) $< -o $@

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
