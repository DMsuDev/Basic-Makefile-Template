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
# Primary Build Configuration
# ──────────────────────────────────────────────────────────────────────────────

# Default build type when running plain `make`
.DEFAULT_GOAL := all

# 0 = Silent Mode; 1 = Color & Details Mode
ifneq ($(filter -j%,$(MAKEFLAGS)),)
    VERBOSE := 0
else
    VERBOSE := 1
endif

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
# Main configuration – customize these variables
# ──────────────────────────────────────────────────────────────────────────────

# Application name (without extension)
APP_NAME ?= MyProject

# Source file extension
SRC_EXT  ?= cpp

# C/C++ standard to use
LANGUAGE ?= c++23
# Define compiler to use
CXX      ?= g++

# Architecture to TARGET (-march=...). Use "native" to optimize for current machine
# Examples: native, skylake, haswell, alderlake, znver4, armv8-a, 64, 32 ...
ARCH ?= native

# Show console window on Windows? (set to false for GUI-only apps)
USE_CONSOLE ?= true

# Optimization levels
OPT_RELEASE ?= -O3
OPT_DEBUG   ?= -Og

# ──────────────────────────────────────────────────────────────────────────────
# Paths – usually no need to change
# ──────────────────────────────────────────────────────────────────────────────

# Can be a list separate by spaces " ": include/ src/ src/core imgui/
SOURCE_DIRS  ?= src include
INCLUDE_DIRS ?= include

BUILD_BASE   := ./build
BIN_DIR      := $(BUILD_BASE)/app
OBJ_DIR      := $(BUILD_BASE)/obj
DEP_DIR      := $(BUILD_BASE)/dep
ASM_DIR      := $(BUILD_BASE)/asm

# ──────────────────────────────────────────────────────────────────────────────
# Compiler & flags
# ──────────────────────────────────────────────────────────────────────────────

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
# Platform detection & common settings
# ──────────────────────────────────────────────────────────────────────────────

ifeq ($(OS),Windows_NT)
    OS_NAME     := Windows
    ifeq ($(USE_CONSOLE),false)
        CONSOLEFLAGS := -mwindows
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
# Libraries & linker flags – main customization area
# ──────────────────────────────────────────────────────────────────────────────

# ── Library search paths (use -L) ──
LDFLAGS ?= -L./lib/

# ── Libraries to link against (use -l) ──
LIBS ?= 

# === Platform-specific libraries ===
ifeq ($(OS),Windows_NT)
    # Windows / MinGW common
    LIBS += -lkernel32 -luser32 -lgdi32 -lshell32 -ldwmapi
endif

ifeq ($(OS_NAME),Linux)
    # Typical Linux dependencies
    # threads (usually needed on Linux)
    # LIBS += -lXrandr -lXinerama -lXcursor -lXi
    LIBS += -lGL -ldl -lpthread 
endif

ifeq ($(OS_NAME),macOS)
    # macOS frameworks (use -framework instead of -l)
    LDFLAGS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
endif

# ── Final linker command ingredients ──
LDFLAGS += $(LIBS) $(CONSOLEFLAGS)

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

# Generate files that encode make rules for the .h dependencies
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

# ──────────────────────────────────────────────────────────────────────────────
# Build rules
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: all dirs
ifeq ($(words $(SOURCES)),0)
all: clean-banner
	@printf "\n$(WARN_COLOR)No source files found. Nothing to build.$(NO_COLOR)\n"
	@exit 0
else
all: clean-banner dirs $(BIN_DIR)/$(TARGET)
endif

dirs:
	@$(MKDIR) "$(call FIXPATH,$(BIN_DIR))"
	@$(MKDIR) "$(call FIXPATH,$(OBJ_DIR))"
	@$(MKDIR) "$(call FIXPATH,$(DEP_DIR))"

$(OBJ_DIR)/%.o : %.$(SRC_EXT) | dirs
ifeq ($(VERBOSE),1)
	@printf "  $(OK_COLOR)Compiling$(NO_COLOR)  %-40s " "$<"
else
	@printf "[Compiling] -> %s\n" "$<"
endif
	@$(MKDIR) "$(@D)" >/dev/null 2>&1
	@$(MKDIR) "$(call FIXPATH,$(dir $(DEP_DIR)/$<))" >/dev/null 2>&1
	@if $(CXX) $(CXXFLAGS) $(INCLUDES) $(DEPFLAGS) -c $< -o $@ 2>compile.log; then \
		if [ "$(VERBOSE)" = "1" ]; then \
			printf "$(OK_COLOR)✓$(NO_COLOR)\n"; \
		fi; \
		rm -f compile.log; \
	else \
		if [ "$(VERBOSE)" = "1" ]; then \
			printf "$(ERROR_COLOR)✗$(NO_COLOR)\n"; \
			printf "  $(ERROR_COLOR)Compilation failed for $<$(NO_COLOR)\n"; \
		else \
			printf "[Error] On compilation $<"; \
		fi; \
		cat compile.log; \
		rm -f compile.log; \
		exit 1; \
	fi

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@printf "\n$(INFO_COLOR)───────$(NO_COLOR) $(HIGHLIGHT)Linking$(NO_COLOR)\n"
	@printf "  %-12s : %s\n" "Target" "$(TARGET)"
	@printf "  %-12s : %s\n" "Objects" "$(words $(OBJECTS))"
	@$(CXX) $^ -o $@ $(LDFLAGS) \
		&& printf "  $(OK_COLOR)✓$(NO_COLOR) %-10s : %s\n" "Status" "Linked successfully" \
		|| printf "  $(ERROR_COLOR)✗$(NO_COLOR) %-10s : %s\n" "Status" "Linking failed"
	@printf "$(INFO_COLOR)────────────────────────────────────────────$(NO_COLOR)\n"

$(DEP_DIR)/%.d: ;
.PRECIOUS: $(DEP_DIR)/%.d

-include $(DEPENDENCIES)

# ──────────────────────────────────────────────────────────────────────────────
# Build UI variants
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: clean-banner
clean-banner:
	@clear 2>/dev/null || cls
ifeq ($(VERBOSE),1)
	@printf "$(INFO_COLOR)─── Build: $(HIGHLIGHT)$(APP_NAME)$(NO_COLOR)\n"
	@printf "  $(BOLD)> OS:$(NO_COLOR)           %s\n" "$(OS_NAME)"
	@printf "  $(BOLD)> Host CPU:$(NO_COLOR)     %s\n" "$(HOST_ARCH)"
	@printf "  $(BOLD)> Target Arch:$(NO_COLOR)  %s\n" "$(TARGET_ARCH)"
	@printf "$(INFO_COLOR)────────────────────────────────────────────$(NO_COLOR)\n"
else
	@printf "Build $(APP_NAME)\n"
	@printf "OS: $(OS_NAME)   |   Host: $(HOST_ARCH)   |   TARGET arch: $(TARGET_ARCH)\n\n"
endif

# ──────────────────────────────────────────────────────────────────────────────
# Build variants
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: debug release 

# Address and Undefined Behavior Sanitizers (only on non-Windows)
SANITIZERS :=
ifneq ($(OS),Windows_NT)
	SANITIZERS := -fsanitize=address,undefined
endif

# Debug build: add debug flags and link with sanitizers, then build
debug: CXXFLAGS += $(OPT_DEBUG) -ggdb3 -DDEBUG
debug: LDFLAGS  += $(SANITIZERS)
debug: all

# Release build: add optimization flags and build
release: CXXFLAGS += $(OPT_RELEASE) -DNDEBUG -march=$(ARCH)
release: all

# ──────────────────────────────────────────────────────────────────────────────
# Assembly & Disassembly
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: asm
asm:
	@printf "\n$(INFO_COLOR)───────$(NO_COLOR) $(HIGHLIGHT)Assembly$(NO_COLOR)\n"
	@printf "  %-12s : %s\n" "Sources" "$(words $(SOURCES))"
	@$(MKDIR) $(call FIXPATH,$(ASM_DIR))
	@$(foreach src,$(SOURCES), \
		$(CXX) -S -masm=intel -O2 $(INCLUDES) -o $(ASM_DIR)/$(notdir $(basename $(src))).s $(src); \
	)
	@printf "  $(OK_COLOR)✓$(NO_COLOR) %-12s : %s\n" "Output" "$(ASM_DIR)"
	@printf "$(INFO_COLOR)────────────────────────────────────────────$(NO_COLOR)\n\n"

.PHONY: disasm
disasm: all
	@printf "\n$(INFO_COLOR)───────$(NO_COLOR) $(HIGHLIGHT)Disassembly$(NO_COLOR)\n"
	@printf "  %-12s : %s\n" "Target" "$(TARGET)"
	@$(MKDIR) $(call FIXPATH,$(ASM_DIR))
	@$(OBJDUMP) -d -Mintel -C "$(call FIXPATH,$(BIN_DIR)/$(TARGET))" > "$(ASM_DIR)/$(APP_NAME)_disasm.txt"
	@$(OBJDUMP) -S -wC "$(call FIXPATH,$(BIN_DIR)/$(TARGET))" > "$(ASM_DIR)/$(APP_NAME)_source.txt"
	@printf "  $(OK_COLOR)✓$(NO_COLOR) %-10s : %s\n" "Output:" "$(ASM_DIR)"
	@printf "$(INFO_COLOR)────────────────────────────────────────────$(NO_COLOR)\n\n"

# ──────────────────────────────────────────────────────────────────────────────
# Run Rules
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: run run-debug
run: release
	@printf "\n$(INFO_COLOR)───────$(NO_COLOR) $(HIGHLIGHT)Run$(NO_COLOR)\n"
	@printf "  %-12s : %s\n" "Target" "$(TARGET)"
	@printf "  $(INFO_COLOR)→ Running...$(NO_COLOR)\n"
	@$(BIN_DIR)/$(TARGET)
	@printf "$(INFO_COLOR)────────────────────────────────────────────$(NO_COLOR)\n\n"

run-debug: debug
	@printf "\n$(INFO_COLOR)───────$(NO_COLOR) $(HIGHLIGHT)Run Debug$(NO_COLOR)\n"
	@printf "  %-12s : %s\n" "Target" "$(TARGET)"
	@printf "  $(INFO_COLOR)→ Running DEBUG...$(NO_COLOR)\n"
	@$(BIN_DIR)/$(TARGET)
	@printf "$(INFO_COLOR)────────────────────────────────────────────$(NO_COLOR)\n\n"

# ──────────────────────────────────────────────────────────────────────────────
# Clean Rules
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: clean full-clean
clean:
	@printf "\n$(INFO_COLOR)───────$(NO_COLOR) $(HIGHLIGHT)Clean$(NO_COLOR)\n"
	@printf "  %-12s : %s\n" "Removing" "OBJ, DEP, ASM, Binary"
	@$(RM) $(OBJ_DIR) $(DEP_DIR) $(ASM_DIR) "$(BIN_DIR)/$(TARGET)"
	@printf "  $(OK_COLOR)✓$(NO_COLOR) %-10s : %s\n" "Done" "$(BUILD_BASE)"
	@printf "$(INFO_COLOR)────────────────────────────────────────────$(NO_COLOR)\n\n"

full-clean:
	@printf "\n$(INFO_COLOR)───────$(NO_COLOR) $(HIGHLIGHT)Full Clean$(NO_COLOR)\n"
	@printf "  %-12s : %s\n" "Removing" "$(BUILD_BASE)"
	@$(RM) "$(BUILD_BASE)"
	@printf "  $(OK_COLOR)✓$(NO_COLOR) %-10s : %s\n" "Done" "$(BUILD_BASE)"
	@printf "$(INFO_COLOR)────────────────────────────────────────────$(NO_COLOR)\n\n"

# ──────────────────────────────────────────────────────────────────────────────
# Help & Info
# ──────────────────────────────────────────────────────────────────────────────

.PHONY: help info

help:
	@printf "\n$(INFO_COLOR)───────$(NO_COLOR) $(HIGHLIGHT)Help$(NO_COLOR)\n"
	@printf "  $(INFO_COLOR)%s$(NO_COLOR)\n" "Common commands:"
	@printf "    %-18s → %s\n" "make"               "release build"
	@printf "    %-18s → %s\n" "make debug"         "debug build + sanitizers"
	@printf "    %-18s → %s\n" "make release"       "optimized build"
	@printf "    %-18s → %s\n" "make run"           "release + run"
	@printf "    %-18s → %s\n" "make asm"           "generate .s files"
	@printf "    %-18s → %s\n" "make disasm"        "disassemble binary"
	@printf "    %-18s → %s\n" "make clean"         "remove objects & binary"
	@printf "    %-18s → %s\n" "make full-clean"    "delete entire build/"
	@printf "\n"
	@printf "  $(INFO_COLOR)%s$(NO_COLOR)\n" "Examples:"
	@printf "    make release ARCH=znver4\n"
	@printf "    make debug   CXX=clang++\n"
	@printf "$(INFO_COLOR)────────────────────────────────────────────$(NO_COLOR)\n\n"


info:
	@printf "\n$(INFO_COLOR)───────$(NO_COLOR) $(HIGHLIGHT)Project Info$(NO_COLOR)\n"
	@printf "  %-12s : %s\n" "Project"      "$(APP_NAME)"
	@printf "  %-12s : %s files\n" "Sources"      "$(words $(SOURCES))"
	@printf "  %-12s : %s\n" "Target"       "$(BIN_DIR)/$(TARGET)"
	@printf "  %-12s : %s\n" "Compiler"     "$(CXX)"
	@printf "  %-12s : %s\n" "Standard"     "$(LANGUAGE)"
	@printf "  %-12s : %s\n" "OS"           "$(OS_NAME)"
	@printf "  %-12s : %s\n" "Host arch"    "$(HOST_ARCH)"
	@printf "  %-12s : %s\n" "Target arch"  "$(TARGET_ARCH)"
	@printf "$(INFO_COLOR)────────────────────────────────────────────$(NO_COLOR)\n\n"

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