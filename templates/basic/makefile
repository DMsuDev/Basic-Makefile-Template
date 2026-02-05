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
LANGUAGE    ?= c++23
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

CXXFLAGS_BASE := -std=$(LANGUAGE) -fdiagnostics-color=always

# Set flags that control and show compiler errors
# Use -Werror to stop compilation on warnings
ERRORFLAGS    := -Wall -Wextra -pedantic-errors

CXXFLAGS      := $(CXXFLAGS_BASE) $(ERRORFLAGS)
LDFLAGS       ?= -L./lib
LIBS          ?=

# Show console window on Windows? (set to false for GUI-only apps)
ifeq ($(USE_CONSOLE),false)
    LDFLAGS += -mwindows
endif

# ─── Platform detection ───────────────────────────────────────────────────────

UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)

RM := rm -rf
ifeq ($(findstring Windows,$(OS)),Windows)
    IS_WINDOWS := yes
    MKDIR      := mkdir
    EXE_EXT    := .exe
else ifeq ($(UNAME_S),Darwin)
    IS_MACOS   := yes
    MKDIR      := mkdir -p
    EXE_EXT    :=
else
    IS_LINUX   := yes
    MKDIR      := mkdir -p
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
	@$(MKDIR) "$(BIN_DIR)" 2>/dev/null || true
	@$(MKDIR) "$(OBJ_DIR)" 2>/dev/null || true
	@$(MKDIR) "$(DEP_DIR)" 2>/dev/null || true
	@mkdir -p "$(DEP_DIR)" 2>/dev/null || true

$(OBJ_DIR)/%.o : %.$(SRC_EXT)
	@printf "[Compiling] → %s\n" "$<"
	@mkdir -p "$(@D)"
	@mkdir -p "$(DEP_DIR)/$(dir $*)"
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
	@cls 2>/dev/null || clear
	@printf "\n═══════════════════════════════════════════\n"
	@printf "  Building: $(APP_NAME)\n"
	@printf "═══════════════════════════════════════════\n\n"

run: release
	@printf "\n───────── Execution\n"
	@$(BIN_DIR)/$(TARGET)

# ─── Cleanup targets ──────────────────────────────────────────────────────────

clean:
	@printf "\n───────── Cleaning\n"
	@$(RM) "$(OBJ_DIR)" 2>/dev/null || true
	@$(RM) "$(DEP_DIR)" 2>/dev/null || true
	@$(RM) "$(BIN_DIR)/$(TARGET)" 2>/dev/null || true
	@printf "  ✓ Cleaned build artifacts\n"

full-clean:
	@printf "\n───────── Full Clean\n"
	@$(RM) "$(BUILD_BASE)" 2>/dev/null || true
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
	@printf "Variables (example: make APP_NAME=MyApp):\n"
	@printf "  APP_NAME    App name (default: $(APP_NAME))\n"
	@printf "  CXX         Compiler (default: $(CXX))\n"
	@printf "  LANGUAGE    C++ standard (default: $(LANGUAGE))\n\n"

info:
	@printf "\n═══════ Project Info ═══════\n"
	@printf "  App Name      : $(APP_NAME)\n"
	@printf "  Target        : $(TARGET)\n"
	@printf "  Compiler      : $(CXX)\n"
	@printf "  C++ Standard  : $(LANGUAGE)\n"
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