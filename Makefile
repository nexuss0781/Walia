# ==============================================================================
# WALIA SOVEREIGN LANGUAGE BUILD ENGINE
# (c) 2025. Persistent, Effect-Managed Architecture.
# ==============================================================================

# 1. COMPILER & PATHS
CC      := gcc
BIN     := walia
SRC_DIR := src
# Explicitly list subdirectories for compatibility
SRC     := $(wildcard $(SRC_DIR)/*.c) \
           $(wildcard $(SRC_DIR)/core/*.c) \
           $(wildcard $(SRC_DIR)/db/*.c) \
           $(wildcard $(SRC_DIR)/vm/*.c) \
           $(wildcard $(SRC_DIR)/lib/*.c) \
           $(wildcard $(SRC_DIR)/sys/*.c) \
           $(wildcard $(SRC_DIR)/web/*.c) \
           $(wildcard $(SRC_DIR)/tooling/*.c) \
           $(wildcard $(SRC_DIR)/drivers/*.c) \
           $(wildcard $(SRC_DIR)/sql/*.c) \
           $(wildcard waliaos/core/*.c) \
           $(wildcard waliaos/drivers/*.c)
OBJ     := $(SRC:.c=.o)

# Detect Environment (Linux vs Termux)
TERMUX := $(shell if [ -d /data/data/com.termux ]; then echo 1; else echo 0; fi)

ifeq ($(TERMUX), 1)
    PREFIX := /data/data/com.termux/files/usr
    INSTALL_DIR := $(PREFIX)/bin
    LIB_DIR := $(PREFIX)/lib/walia
else
    PREFIX := /usr/local
    INSTALL_DIR := $(PREFIX)/bin
    LIB_DIR := $(PREFIX)/lib/walia
endif

# 2. ARCHITECTURE-SPECIFIC OPTIMIZATION (UFO SPEED)
ARCH := $(shell uname -m)
CFLAGS := -I$(SRC_DIR) -Iinclude -Iwaliaos/include -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200809L

# Optimization Level 3 + Link Time Optimization (LTO)
CFLAGS += -O3 -flto -fno-strict-aliasing

# SIMD Detection Logic
ifeq ($(ARCH), x86_64)
    # Enable AVX-512 and AVX2 for Intel/AMD
    CFLAGS += -march=native -mavx512f -mavx2 -mfma
else ifeq ($(ARCH), aarch64)
    # Enable NEON for ARM64 (Apple Silicon / Graviton / Android)
    CFLAGS += -march=native -mfpu=neon
endif

# 3. LINKER FLAGS
LDFLAGS := -lm -lpthread -flto

# 4. BUILD RULES
.PHONY: all clean install uninstall test debug

all: $(BIN)
	@echo ">> Walia: Sovereign Binary compiled for $(ARCH) successfully."

$(BIN): $(OBJ)
	@$(CC) $(OBJ) -o $@ $(LDFLAGS)
	@echo ">> Walia: UFO-Grade Optimization Applied [LTO enabled]."

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

# 5. INSTALLATION (One-Command Deployment)
install: $(BIN)
	@echo ">> Walia: Installing to $(INSTALL_DIR)..."
	@mkdir -p $(INSTALL_DIR)
	@mkdir -p $(LIB_DIR)
	@cp $(BIN) $(INSTALL_DIR)/$(BIN)
	@chmod +x $(INSTALL_DIR)/$(BIN)
	@echo ">> Walia: Installation Complete. Run 'walia' to begin."

uninstall:
	@rm -f $(INSTALL_DIR)/$(BIN)
	@rm -rf $(LIB_DIR)
	@echo ">> Walia: Sovereign environment removed."

# 6. QUALITY ASSURANCE & DIAGNOSTICS
test: $(BIN)
	@./$(BIN) --test

debug: CFLAGS += -g -DDEBUG_TRACE_EXECUTION -DDEBUG_PRINT_CODE
debug: clean all
	@echo ">> Walia: Debug build complete with verbose tracing."

clean:
	@rm -f $(OBJ) $(BIN)
	@echo ">> Walia: Build artifacts purged."

# 7. HELP (Developer Experience)
help:
	@echo "Walia Sovereign SDK Build System"
	@echo "Usage:"
	@echo "  make            Build optimized binary"
	@echo "  make install    Install to system (requires sudo/root)"
	@echo "  make test       Run integrated Walia test suite"
	@echo "  make clean      Remove build files"
	@echo "  make debug      Build with full diagnostic tracing"
