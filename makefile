# Compiler and flags
CC = gcc
CFLAGS = -Wall  -Wextra -g `pkg-config fuse --cflags --libs`

# Directories
SRCDIR := ./src
INCDIR := ./src
BUILDDIR := ./build
TARGETDIR := ./bin

# Target executable name
TARGET := $(TARGETDIR)/fs

# Source files and object files
SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

# Include directories
INCLUDES := -I$(INCDIR)

# Linker flags
LDFLAGS = `pkg-config fuse --cflags --libs`

# Rules
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(TARGETDIR)
	$(CC) $^ -o $@ $(LDFLAGS) 

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -rf $(BUILDDIR) $(TARGETDIR)
