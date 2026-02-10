# ==============================================================================
# Basic C++ Makefile Template
# Author: Dayron Mustelier (@DMsuDev) - 2025/2026
# License: MIT
# ==============================================================================

# Default build type when running plain `make`
.DEFAULT_GOAL := all

# Ensure consistent behavior across make versions
.SUFFIXES:

# ─── Main configuration ───────────────────────────────────────────────────────

APP_NAME    ?= ProjectName
SRC_EXT     ?= cpp
CXX_STD     ?= c++23
CXX         ?= g++
USE_CONSOLE ?= true

# ─── Directories & Paths ──────────────────────────────────────────────────────

# Can be a list separated by spaces: src include src/core
SOURCE_DIRS   ?= src
INCLUDE_DIRS  ?= include

BUILD_BASE    := ./build
BIN_DIR       := $(BUILD_BASE)/app
OBJ_DIR       := $(BUILD_BASE)/obj
DEP_DIR       := $(BUILD_BASE)/dep

# ─── Compiler & Linker flags (base) ─────────────────────────────────────────────

CXXFLAGS_BASE := -std=$(CXX_STD) -fdiagnostics-color=always

# Warning Level Configuration
# Options: minimal, normal, strict (default: minimal)
# - minimal:   Only -Wall -Wextra -pedantic-errors
# - normal:    + type conversion and format checks
# - strict:    + logic, safety, and quality warnings
WARN_LEVEL ?= minimal

# Base warnings (always applied)
ERROFFLAGS := -Wall -Wextra -pedantic-errors

# Type conversion warnings
WARN_CONVERSION := -Wconversion -Wsign-conversion -Wdouble-promotion

# Logic and correctness warnings
WARN_LOGIC := -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wrestrict

# Safety warnings
WARN_SAFETY := -Wnull-dereference -Wformat=2 -Wunreachable-code

# Code quality warnings
WARN_QUALITY := -Wshadow -Wunused -Wunused-parameter

# Apply warning level based on WARN_LEVEL setting
ifeq ($(WARN_LEVEL),strict)
	ERROFFLAGS += $(WARN_CONVERSION) $(WARN_LOGIC) $(WARN_SAFETY) $(WARN_QUALITY)
else ifeq ($(WARN_LEVEL),normal)
	ERROFFLAGS += $(WARN_CONVERSION) $(WARN_LOGIC) $(WARN_SAFETY)
else ifeq ($(WARN_LEVEL),minimal)
	# Only base flags
else
	$(warning WARNING: Unknown WARN_LEVEL='$(WARN_LEVEL)', using 'strict')
	WARN_LEVEL := strict
	ERROFFLAGS += $(WARN_CONVERSION) $(WARN_LOGIC) $(WARN_SAFETY) $(WARN_QUALITY)
endif

# Convert warnings to errors in release mode
# -Werror: Treat compiler warnings as errors (ensures code quality)
#   - Only applies in release builds to catch potential bugs early
#   - Debug builds keep warnings as warnings for faster iteration
#   - Release builds fail if any warning occurs (prevents shipping buggy code)
ifeq (release,$(findstring release,$(BUILD_TYPE)))
    #ERROFFLAGS += -Werror
endif

CXXFLAGS      := $(CXXFLAGS_BASE) $(ERRORFLAGS)
LDFLAGS       ?= -L./lib
LIBS          ?=

# Show console window on Windows? (set to false for GUI-only apps)
ifeq ($(USE_CONSOLE),false)
    LDFLAGS += -mwindows
endif

# ─── Platform detection ───────────────────────────────────────────────────────

UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)

ifeq ($(findstring Windows,$(OS)),Windows)
    # Choose command style depending on shell (PowerShell vs cmd)
    ifdef PSModulePath
        RM      := rm -rf
        MKDIR   := mkdir -p
        FIXPATH = $1
    else
        RM      := rmdir /S /Q
        MKDIR   := mkdir
        FIXPATH = $(subst /,\,$1)
    endif

    IS_WINDOWS := yes
    EXE_EXT    := .exe

else ifeq ($(UNAME_S),Darwin)
    IS_MACOS   := yes
    RM         := rm -rf
    MKDIR      := mkdir -p
    FIXPATH    = $1
    EXE_EXT    :=

else ifeq ($(UNAME_S),Linux)
    IS_LINUX   := yes
    RM         := rm -rf
    MKDIR      := mkdir -p
    FIXPATH    = $1
    EXE_EXT    :=

else
    # Fallback for unknown systems
    RM         := rm -rf
    MKDIR      := mkdir -p
    FIXPATH    = $1
    EXE_EXT    :=
endif

TARGET := $(APP_NAME)$(EXE_EXT)

# ─── Automatic source & object discovery ──────────────────────────────────────

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

# Generate files that encode make rules for header dependencies
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

# ─── Build rules ──────────────────────────────────────────────────────────────

.PHONY: all dirs clean clean-banner run debug release help info

all: clean-banner dirs $(BIN_DIR)/$(TARGET)

dirs:
	@$(MKDIR) "$(call FIXPATH,$(BIN_DIR))" 2>/dev/null || true
	@$(MKDIR) "$(call FIXPATH,$(OBJ_DIR))" 2>/dev/null || true
	@$(MKDIR) "$(call FIXPATH,$(DEP_DIR))" 2>/dev/null || true

$(OBJ_DIR)/%.o : %.$(SRC_EXT)
	@printf "[Compiling] → %s\n" "$<"
	@$(MKDIR) "$(call FIXPATH,$(@D))"
	@$(MKDIR) "$(call FIXPATH,$(DEP_DIR)/$(dir $*))"
	@if $(CXX) $(CXXFLAGS) $(INCLUDES) $(DEPFLAGS) -c $< -o $@; then \
		printf "  ✓ Success\n"; \
	else \
		printf "  ✗ Error\n"; \
		exit 1; \
	fi

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@printf "\n───────── Linking\n"
	@printf "  %-15s: %s\n" "Target" "$(TARGET)"
	@printf "  %-15s: %s\n" "Objects" "$(words $(OBJECTS))"
	@if $(CXX) $^ -o $@ $(LDFLAGS) $(LIBS); then \
		printf "  ✓ Linked successfully\n"; \
	else \
		printf "  ✗ Linking failed\n"; \
		exit 1; \
	fi

# Only include existing dependency files - avoids infinite loops
-include $(wildcard $(DEPENDENCIES))

# ─── Build variants ───────────────────────────────────────────────────────────

# Address and Undefined Behavior Sanitizers (only on non-Windows)
SANITIZERS :=
ifneq ($(OS),Windows_NT)
	SANITIZERS := -fsanitize=address,undefined
endif

debug:   CXXFLAGS += -Og -ggdb3 -DDEBUG
debug:   LDFLAGS += $(SANITIZERS)
debug:   all

release: CXXFLAGS += -O3 -DNDEBUG -march=native
release: all

# ─── Display targets ──────────────────────────────────────────────────────────

clean-banner:
	@if [ -t 1 ]; then \
		if command -v cls >/dev/null 2>&1; then cls; else clear; fi; \
	fi
	@printf "\n═══════════════════════════════════════════\n"
	@printf "  Building: $(APP_NAME)\n"
	@printf "═══════════════════════════════════════════\n\n"

run: release
	@printf "\n───────── Execution\n"
	@$(BIN_DIR)/$(TARGET)

# ─── Cleanup targets ──────────────────────────────────────────────────────────

clean:
	@printf "\n───────── Cleaning\n"
	@$(RM) "$(call FIXPATH,$(OBJ_DIR))" 2>/dev/null || true
	@$(RM) "$(call FIXPATH,$(DEP_DIR))" 2>/dev/null || true
	@$(RM) "$(call FIXPATH,$(BIN_DIR)/$(TARGET))" 2>/dev/null || true
	@printf "  ✓ Cleaned build artifacts\n"

full-clean:
	@printf "\n───────── Full Clean\n"
	@$(RM) "$(call FIXPATH,$(BUILD_BASE))" 2>/dev/null || true
	@printf "  ✓ Removed build directory\n"

# ─── Info targets ─────────────────────────────────────────────────────────────

help:
	@printf "\n═══════ C++ Build System ═══════\n"
	@printf "Usage: make [target] [VAR=value]\n\n"
	@printf "Targets:\n"
	@printf "  all         Build project (default)\n"
	@printf "  debug       Debug build + sanitizers\n"
	@printf "  release     Optimized release build\n"
	@printf "  run         Build & execute\n"
	@printf "  clean       Remove objects & binary\n"
	@printf "  full-clean  Delete entire build/\n"
	@printf "  info        Show configuration\n"
	@printf "  help        This message\n\n"
	@printf "Configuration Variables:\n"
	@printf "  APP_NAME      App name (default: $(APP_NAME))\n"
	@printf "  CXX           Compiler (default: $(CXX))\n"
	@printf "  CXX_STD       C++ standard (default: $(CXX_STD))\n"
	@printf "  WARN_LEVEL    Warning level: minimal, normal, strict (default: strict)\n\n"
	@printf "Examples:\n"
	@printf "  make debug                      # Debug with all warnings\n"
	@printf "  make WARN_LEVEL=minimal         # Minimal warnings\n"
	@printf "  make WARN_LEVEL=normal CXX=clang++ # Balanced + clang\n"

info:
	@printf "\n═══════ Project Info ═══════\n"
	@printf "  App Name      : $(APP_NAME)\n"
	@printf "  Target        : $(TARGET)\n"
	@printf "  Compiler      : $(CXX)\n"
	@printf "  C++ Standard  : $(CXX_STD)\n"
	@printf "  Sources       : $(words $(SOURCES))\n"
	@printf "  Source Dirs   : $(SOURCE_DIRS)\n"
	@printf "  Include Dirs  : $(INCLUDE_DIRS)\n"
	@printf "  Build Base    : $(BUILD_BASE)\n\n"

# ──────────────────────────────────────────────────────────────────────────────
#                                   COMPILER FLAGS REFERENCE
# ──────────────────────────────────────────────────────────────────────────────
# -I{dir}     Include directory search path
# -L{dir}     Library search path
# -l{lib}     Link against library
# -std={std}  C++ standard (c++11, c++17, c++23, etc.)
# -Wall       Enable most warnings
# -Wextra     Enable extra warnings
# -O0, -O1, -O2, -O3   Optimization levels
# -Os         Optimize for size
# -ggdb3      Generate debug info
# -march=native   Optimize for current CPU
# -fdiagnostics-color=always  Colored compiler output