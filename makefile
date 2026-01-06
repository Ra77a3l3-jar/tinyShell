# Compiler flags
CC      ?= clang
CFLAGS  ?= -Wall -Wextra -g
DBGFLAGS = -g
RELFLAGS = -O2

# Target binary names
TARGET_DEBUG   = tinyShell_debug
TARGET_RELEASE = tinyShell_release

# Source and build directories
SRCDIR   = src
INCDIR   = include
BUILDDIR = build

# Collect all .c files automatically
SRCS     := $(wildcard $(SRCDIR)/*.c)
OBJS     := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))

# Default target
all: debug

# Debug build
debug: CFLAGS += $(DBGFLAGS)
debug: $(TARGET_DEBUG)

$(TARGET_DEBUG): $(OBJS)
	@echo "[LD] $@"
	$(CC) $(CFLAGS) -o $@ $^

# Relese
release: CFLAGS += $(RELFLAGS)
release: $(TARGET_RELEASE)

$(TARGET_RELEASE): $(OBJS)
	@echo "[LD] $@"
	$(CC) $(CFLAGS) -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	@echo "[CC] $<"
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

run: debug
	./$(TARGET_DEBUG)

clean:
	rm -rf $(BUILDDIR) $(TARGET_DEBUG) $(TARGET_RELEASE)

.PHONY: all debug release run clean
