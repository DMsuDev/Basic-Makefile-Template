# ==============================================================================
# Basic C++ Makefile Template
# Author: Dayron Mustelier (@DMsuDev) - 2025/2026
# License: MIT
# ==============================================================================

# Default build type when running plain `make`
.DEFAULT_GOAL := all

# ──────────────────────────────────────────────────────────────────────────────
# Main configuration – customize these variables
# ──────────────────────────────────────────────────────────────────────────────

APP_NAME    ?= MyProject
SRC_EXT     ?= cpp
LANGUAGE    ?= c++23
CXX         ?= g++
USE_CONSOLE ?= true

# ──────────────────────────────────────────────────────────────────────────────
# Directories & Paths
# ──────────────────────────────────────────────────────────────────────────────

# Can be a list separate by spaces " ": include/ src/ src/core imgui/
SOURCE_DIRS   ?= src
INCLUDE_DIRS  ?= include

BUILD_BASE    := build
BIN_DIR       := $(BUILD_BASE)/bin
OBJ_DIR       := $(BUILD_BASE)/obj
DEP_DIR       := $(BUILD_BASE)/dep

# ──────────────────────────────────────────────────────────────────────────────
# Compiler & Linker flags (base)
# ──────────────────────────────────────────────────────────────────────────────

CXXFLAGS_BASE := -std=$(LANGUAGE) -fdiagnostics-color=always

# Set flags that control and shows the error of the compiler
# If type -Werror the compiler when throw error message stop the compilation.
ERRORFLAGS    := -Wall -Wextra -pedantic-errors

CXXFLAGS      := $(CXXFLAGS_BASE) $(ERRORFLAGS)
LDFLAGS       := 

# Show console window on Windows? (set to false for GUI-only apps) (Windows)
ifeq ($(USE_CONSOLE),false)
    LDFLAGS += -mwindows
    # LDFLAGS += -luser32 -lgdi32
endif

# ──────────────────────────────────────────────────────────────────────────────
# Automatic source & object discovery
# ──────────────────────────────────────────────────────────────────────────────

# Find all source files recursively
define recurse
$(wildcard $(1)/*.$(SRC_EXT)) \
$(foreach d,$(wildcard $(1)/*), \
  $(if $(wildcard $(d)/),$(call recurse,$(d))))
endef

SOURCES      := $(foreach dir,$(SOURCE_DIRS),$(call recurse,$(dir)))
OBJECTS      := $(patsubst %.$(SRC_EXT),$(OBJ_DIR)/%.o,$(SOURCES))
DEPENDENCIES := $(OBJECTS:.o=.d)

INCLUDES     := $(addprefix -I,$(INCLUDE_DIRS))
LIB_DIRS     ?= -L./lib
LIBS         ?=

LDFLAGS      += $(LIB_DIRS) $(addprefix -l,$(LIBS))

# Generate files that encode make rules for the .h dependencies
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

# ──────────────────────────────────────────────────────────────────────────────
# Build rules
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: all dirs

all: clean-banner dirs $(BIN_DIR)/$(APP_NAME)

dirs:
	@mkdir -p $(BIN_DIR) $(OBJ_DIR) $(DEP_DIR)

$(OBJ_DIR)/%.o : %.$(SRC_EXT) | dirs
	@printf "[Compiling] -> %s\n" "$<"
	@mkdir -p $(@D) $(dir $(DEP_DIR)/$*.d) 2>/dev/null || true
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEPFLAGS) -c $< -o $@ 2>compile.log || \
		{ printf "\n[ERROR] Compilation failed for $<\n"; cat compile.log; rm -f compile.log; exit 1; }
	@rm -f compile.log

$(BIN_DIR)/$(APP_NAME): $(OBJECTS)
	@printf "\n─── Linking → %s\n" "$@"
	@$(CXX) $^ -o $@ $(LDFLAGS) && \
		printf "  %-12s: Linked successfully\n" "Status" || \
		{ printf "  %-12s: Linking failed\n" "Status"; exit 1; }

$(DEP_DIR)/%.d: ;
.PRECIOUS: $(DEP_DIR)/%.d

-include $(DEPENDENCIES)

# ──────────────────────────────────────────────────────────────────────────────
# Build variants
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: debug release
debug:   CXXFLAGS += -Og -g -ggdb3 -DDEBUG
debug:   all

release: CXXFLAGS += -O3 -DNDEBUG -march=native
release: all

# ──────────────────────────────────────────────────────────────────────────────
# Utility targets
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: clean-banner run clean help

clean-banner:
	@clear 2>/dev/null || cls

run: release
	@$(BIN_DIR)/$(APP_NAME)

clean:
	@rm -rf $(BUILD_BASE)
	@printf "Cleaned build directory\n"

help:
	@echo "Available targets:"
	@echo "  make [all]     → Build (default)"
	@echo "  make debug     → Build with debug symbols"
	@echo "  make release   → Build optimized"
	@echo "  make run       → Build release + execute"
	@echo "  make clean     → Remove build artifacts"
	@echo "  make help      → Show this help"