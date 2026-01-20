# ==============================================================================
# Modern C++ Project Makefile Template
# Author: Dayron Mustelier (@DMsuDev) - 2025/2026
# License: MIT
# Purpose: Clean, cross-platform template for C++ applications / libraries
# Supported platforms: Windows (MinGW-w64/MSYS2), Linux, macOS
# Features: Debug/Release, dependency tracking, architecture selection,
#           assembly output, disassembly, clean & informative output
# ==============================================================================

# ──────────────────────────────────────────────────────────────────────────────
# 0. Primary Build Configuration
# ──────────────────────────────────────────────────────────────────────────────

# 0 = Silent Mode; 1 = Color & Details Mode
ifneq ($(filter -j%,$(MAKEFLAGS)),)
    VERBOSE := 0
else
    VERBOSE := 1
endif

# ──────────────────────────────────────────────────────────────────────────────
# 1. Main configuration – customize these variables
# ──────────────────────────────────────────────────────────────────────────────

# Application/library name (without extension)
APP_NAME ?= MyProject

# Source file extension
SRC_EXT ?= cpp

# C/C++ standard to use
LANGUAGE ?= c++23

# Define compiler to use
CXX ?= g++

# Default build type when running plain `make`
.DEFAULT_GOAL := run

# Architecture to TARGET (-march=...). Use "native" to optimize for current machine
# Examples: native, skylake, haswell, alderlake, znver4, armv8-a, 64, 32 ...
ARCH ?= native

# Show console window on Windows? (set to false for GUI-only apps)
USE_CONSOLE ?= true

# Optimization levels
OPT_RELEASE ?= -O3
OPT_DEBUG   ?= -O0

# ──────────────────────────────────────────────────────────────────────────────
# 1.5 Colors (ANSI escape codes) – used for pretty output
# ──────────────────────────────────────────────────────────────────────────────

ifeq ($(VERBOSE),1)
    NO_COLOR     := \033[0m
    BOLD         := \033[1m
    OK_COLOR     := \033[32;01m
    WARN_COLOR   := \033[33;01m
    ERROR_COLOR  := \033[31;01m
    INFO_COLOR   := \033[36;01m
    TITLE_COLOR  := \033[35;01m
    HIGHLIGHT    := \033[95;01m
else
    NO_COLOR     :=
    BOLD         :=
    OK_COLOR     :=
    WARN_COLOR   :=
    ERROR_COLOR  :=
    INFO_COLOR   :=
    TITLE_COLOR  :=
    HIGHLIGHT    :=
endif

# ──────────────────────────────────────────────────────────────────────────────
# 2. Paths – usually no need to change
# ──────────────────────────────────────────────────────────────────────────────

# Can be a list separate by spaces " ": include/ src/ src/core imgui/
SOURCE_DIRS  ?= src src/core
INCLUDE_DIRS ?= include

BUILD_BASE  ?= ./build
BIN_DIR     ?= $(BUILD_BASE)/app
OBJ_DIR     ?= $(BUILD_BASE)/obj
DEP_DIR     ?= $(BUILD_BASE)/dep
ASM_DIR     ?= $(BUILD_BASE)/asm

# Find all source files recursively
SOURCES      := $(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.$(SRC_EXT)))
OBJECTS      := $(patsubst %.$(SRC_EXT),$(OBJ_DIR)/%.o,$(SOURCES))

DEPENDENCIES := $(OBJECTS:.o=.d)
INCLUDES     := $(foreach dir,$(INCLUDE_DIRS),-I$(dir))

# ──────────────────────────────────────────────────────────────────────────────
# 3. Libraries & linker flags – main customization area
# ──────────────────────────────────────────────────────────────────────────────

# ── Library search paths (use -L) ──
LDFLAGS := -L./lib/

LIBS :=

# === Platform-specific libraries ===
ifeq ($(OS),Windows_NT)
    # Windows / MinGW common
    LIBS += -lgdi32 -lopengl32
    # LIBS += -lglfw3dll                    # dynamic GLFW (most common)
    # LIBS += -luser32 -lkernel32 -lwinmm   # useful libs (Only MinGW)
endif

ifeq ($(OS_NAME),Linux)
    # Typical Linux dependencies
    # threads (usually needed on Linux)
    # LIBS += -lXrandr -lXinerama -lXcursor -lXi
    LIBS += -lGL -ldl -lpthread 
endif

ifeq ($(OS_NAME),macOS)
    # macOS frameworks (use -framework instead of -l)
    LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
endif

# ── Final linker command ingredients ──
LDFLAGS += $(LIBS) $(CONSOLEFLAGS)

# ──────────────────────────────────────────────────────────────────────────────
# 4. Compiler & flags
# ──────────────────────────────────────────────────────────────────────────────

# Generate files that encode make rules for the .h dependencies
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

# Set flags that control and shows the error of the compiler
# If type -Werror the compiler when throw error message stop the compilation.
ERRORFLAGS := -Wall -Wextra -pedantic-errors

ifeq ($(CXX),cl)
    # MSVC path (rarely used in this template)
    CXXFLAGS := /std:$(LANGUAGE) /W4 /WX
    OBJDUMP  := dumpbin /DISASM
else
    # GCC / Clang path (most common)
    CXXFLAGS := -std=$(LANGUAGE) -fdiagnostics-color=always $(ERRORFLAGS)
    CXXFLAGS += -Wno-unknown-pragmas -Wno-comment
    OBJDUMP  := objdump
endif

# ──────────────────────────────────────────────────────────────────────────────
# 5. Platform detection & common settings
# ──────────────────────────────────────────────────────────────────────────────

ifeq ($(OS),Windows_NT)
    OS_NAME     := Windows
    ifeq ($(USE_CONSOLE),false)
        CONSOLEFLAGS := -mwindows -luser32
    endif

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

    # fallback – better detection below
    HOST_ARCH ?= $(shell powershell -NoProfile -Command "[System.Environment]::GetEnvironmentVariable('PROCESSOR_ARCHITECTURE')")
    ifeq ($(HOST_ARCH),AMD64)
        HOST_ARCH := x86_64
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        OS_NAME := Linux
    endif
    ifeq ($(UNAME_S),Darwin)
        OS_NAME := macOS
    endif
    RM          := rm -rf
    MKDIR       := mkdir -p
    FIXPATH     = $1
	HOST_ARCH   ?= $(shell uname -m)
endif

TARGET_ARCH := $(if $(filter native,$(ARCH)),$(HOST_ARCH),$(ARCH))
TARGET := $(APP_NAME)-$(TARGET_ARCH)$(if $(filter Windows,$(OS_NAME)),.exe,)

# ──────────────────────────────────────────────────────────────────────────────
# 6. Build rules
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: all dirs
all: dirs $(BIN_DIR)/$(TARGET)

dirs:
	@$(MKDIR) "$(call FIXPATH,$(BIN_DIR))"
	@$(MKDIR) "$(call FIXPATH,$(OBJ_DIR))"
	@$(MKDIR) "$(call FIXPATH,$(DEP_DIR))"

$(OBJ_DIR)/%.o : %.$(SRC_EXT) | dirs
	@echo "[Compiling]  $<  →  $@"
	@$(MKDIR) "$(@D)"
	@$(MKDIR) "$(call FIXPATH,$(DEP_DIR)/$(dir $<))"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEPFLAGS) -c $< -o $@

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@echo "[Linking]    $@"
	@$(CXX) $^ -o $@ $(LDFLAGS)
	@echo "[Completed]  $(TARGET) successful — all symbols resolved."

$(DEP_DIR)/%.d: ;
.PRECIOUS: $(DEP_DIR)/%.d

-include $(DEPENDENCIES)

# ──────────────────────────────────────────────────────────────────────────────
# 7. Build variants
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: debug release clean-banner

SANITIZERS :=
ifneq ($(OS),Windows_NT)
SANITIZERS := -fsanitize=address,undefined
endif

debug: CXXFLAGS += $(OPT_DEBUG) -ggdb3 -DDEBUG
debug: LDFLAGS  += $(SANITIZERS)
debug: clean-banner all

release: CXXFLAGS += $(OPT_RELEASE) -DNDEBUG -march=$(ARCH)
release: clean-banner all

clean-banner:
	@clear || cls
	@echo "Building $(APP_NAME)"
	@echo "OS: $(OS_NAME)   |   Host: $(HOST_ARCH)   |   TARGET arch: $(TARGET_ARCH)"

# ──────────────────────────────────────────────────────────────────────────────
# 8. Utilities
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: asm
asm:
	@echo "Generating assembly files..."
	@$(MKDIR) $(call FIXPATH,$(ASM_DIR))
	@$(foreach src,$(SOURCES), \
		$(CXX) -S -masm=intel -O2 $(INCLUDES) -o $(ASM_DIR)/$(notdir $(basename $(src))).s $(src); \
	)
	@echo "Done → $(ASM_DIR)/"

.PHONY: disasm
disasm: all
	@echo "Disassembling $(TARGET_FULL_FULL)..."
	@$(MKDIR) $(call FIXPATH,$(ASM_DIR))
	@$(OBJDUMP) -d -Mintel -C "$(call FIXPATH,$(BIN_DIR)/$(TARGET))" > "$(ASM_DIR)/$(APP_NAME)_disasm.txt"
	@$(OBJDUMP) -S -wC "$(call FIXPATH,$(BIN_DIR)/$(TARGET))" > "$(ASM_DIR)/$(APP_NAME)_source.txt"
	@echo "Done → $(ASM_DIR)/"

# ──────────────────────────────────────────────────────────────────────────────
# Run TARGET_FULLs
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: run run-debug
run: release
	@echo ""
	@echo "→ Running: $(TARGET)"
	@$(BIN_DIR)/$(TARGET) || $(BIN_DIR)/$(TARGET)

run-debug: debug
	@echo ""
	@echo "→ Running DEBUG: $(TARGET)"
	@$(BIN_DIR)/$(TARGET) || $(BIN_DIR)/$(TARGET)

# ──────────────────────────────────────────────────────────────────────────────
# Clean TARGET_FULLs
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: clean full-clean
clean:
	@echo "Cleaning artifacts..."
	@$(RM) $(OBJ_DIR) $(DEP_DIR) $(ASM_DIR) "$(BIN_DIR)/$(TARGET)"

full-clean:
	@echo "Removing entire build directory..."
	@$(RM) "$(BUILD_BASE)"

# ──────────────────────────────────────────────────────────────────────────────
# 9. Help & info
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: help info
help:
	@echo "═══════════════════════════════════════════════════════════════"
	@echo " $(APP_NAME) – Modern C++ Makefile"
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "Common commands:"
	@echo "  make               → release build"
	@echo "  make debug         → debug build + sanitizers"
	@echo "  make release       → optimized build"
	@echo "  make run           → release + run"
	@echo "  make asm           → generate .s files"
	@echo "  make disasm        → disassemble binary"
	@echo "  make clean         → remove objects & binary"
	@echo "  make full-clean    → delete entire build/"
	@echo ""
	@echo "Examples:"
	@echo "  make release ARCH=znver4"
	@echo "  make debug CXX=clang++"
	@echo "═══════════════════════════════════════════════════════════════"


info:
	@echo "Project       : $(APP_NAME)"
	@echo "Sources       : $(words $(SOURCES)) files"
	@echo "TARGET        : $(BIN_DIR)/$(TARGET)"
	@echo "Compiler      : $(CXX)"
	@echo "Standard      : $(LANGUAGE)"
	@echo "OS            : $(OS_NAME)"
	@echo "Host arch     : $(HOST_ARCH)"
	@echo "TARGET arch   : $(TARGET_ARCH)"

# ──────────────────────────────────────────────────────────────────────────────
#                                      HINTS
# ──────────────────────────────────────────────────────────────────────────────

# -I{directory}   Adds the specified directory to the compiler's header search path (.h, .hpp).
# -l{name}        Instructs the linker to link against the specified library (e.g., -lm, -lstdc++).
# -L{directory}   Adds a directory to the library search path for the linker.

# -Wall           Enables most common compiler warnings.
# -Wextra         Enables additional, more strict warnings.
# -w              Disables all compiler warnings (not recommended).

# -s              Strips symbols from the final executable, reducing size and making analysis harder.
#                 (Note: In MinGW, -static is the flag that generates a static executable, not -s.)

# -static         Links the executable statically (MinGW). Produces a larger but self‑contained binary.

# -O0             No optimization; fastest compilation, easiest debugging.
# -O1, -O2        Moderate optimizations that improve performance without aggressive transformations.
# -Os             Optimizes for size instead of speed.
# -O3             High‑level optimizations; may increase binary size.
# -Ofast          Most aggressive optimizations; may break strict standards compliance.

# -o {file}       Specifies the output file name (e.g., -o program.exe).

# -shared         Produces a shared library (DLL) instead of an executable.

# -municode       Enables Unicode entry point on MinGW (allows using wWinMain instead of main).

# Makefile automatic variables:
# $^              Expands to all prerequisites of the rule.
# $<              Expands to the first prerequisite.
# $@              Expands to the target name.
# @               Suppresses command echoing in the terminal for cleaner output.