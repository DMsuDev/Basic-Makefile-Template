# ==============================================================================
# Modern C++ Project Makefile Template
# Author: Dayron Mustelier (@DMsuDev) - 2025/2026
# License: MIT
# Purpose: Clean, cross-platform template for C++ applications / libraries
# Supported platforms: Windows (MinGW-w64/MSYS2), Linux, macOS
# Features: Debug/Release, sanitizers, static analysis, benchmarking, 
#           documentation, assembly output, disassembly
# ==============================================================================

# Default build type when running plain `make`
.DEFAULT_GOAL := all

# Ensure consistent behavior across make versions
.SUFFIXES:

# ─── Main configuration options ───────────────────────────────────────────────

APP_NAME    ?= ProjectName
SRC_EXT     ?= cpp
CXX_STD     ?= c++23
CXX         ?= g++
OPT_RELEASE ?= -O3
OPT_DEBUG   ?= -Og
BUILD_TYPE  ?= release
USE_LTO     ?= true
ANALYZE     ?= false
USE_CONSOLE ?= true

# Architecture to TARGET (-march=...). Use "native" to optimize for current machine
# Examples: native, skylake, haswell, alderlake, znver4, armv8-a, 64, 32 ...
ARCH ?= native

# ─── Directory Structure ──────────────────────────────────────────────────────

# Can be a list separate by spaces " ": include/ src/ src/core imgui/
SOURCE_DIRS  ?= src include
INCLUDE_DIRS ?= include

BUILD_BASE   := ./build
BIN_DIR      := $(BUILD_BASE)/app
OBJ_DIR      := $(BUILD_BASE)/obj
DEP_DIR      := $(BUILD_BASE)/dep
ASM_DIR      := $(BUILD_BASE)/asm
BENCH_DIR    := $(BUILD_BASE)/benchmark
DOC_BUILD    := $(BUILD_BASE)/docs

# ─── Compiler Configuration ───────────────────────────────────────────────────

# Warning Level Configuration
# Options: minimal, normal, strict (default: minimal)
# - minimal:   Only -Wall -Wextra -pedantic-errors
# - normal:    + type conversion and format checks
# - strict:    + logic, safety, and quality warnings
WARN_LEVEL ?= minimal

# Base warnings (always applied)
ERRORFLAGS := -Wall -Wextra -pedantic-errors

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
	ERRORFLAGS += $(WARN_CONVERSION) $(WARN_LOGIC) $(WARN_SAFETY) $(WARN_QUALITY)
else ifeq ($(WARN_LEVEL),normal)
	ERRORFLAGS += $(WARN_CONVERSION) $(WARN_LOGIC) $(WARN_SAFETY)
else ifeq ($(WARN_LEVEL),minimal)
    # Only base flags
else
	$(warning WARNING: Unknown WARN_LEVEL='$(WARN_LEVEL)', using 'strict')
	WARN_LEVEL := strict
	ERRORFLAGS += $(WARN_CONVERSION) $(WARN_LOGIC) $(WARN_SAFETY) $(WARN_QUALITY)
endif

# Convert warnings to errors in release mode
# -Werror: Treat compiler warnings as errors (enforces code quality standards)
#   RELEASE builds: Warnings = Build FAILS (no buggy code reaches production)
#   DEBUG builds:   Warnings = Messages only (faster iteration during development)
#   relwithdebinfo: Same as release (strict mode with debug symbols)
ifeq ($(BUILD_TYPE),debug)
	# Debug: keep warnings as warnings for easier development
else
    #ERRORFLAGS += -Werror
endif

ifeq ($(CXX),cl)
    # MSVC
    CXXFLAGS := /std:$(CXX_STD) /W4 /WX
    OBJDUMP  := dumpbin /DISASM
else
    # GCC / Clang path (most common)
    CXXFLAGS := -std=$(CXX_STD) -fdiagnostics-color=always $(ERRORFLAGS)
    CXXFLAGS += -Wno-unknown-pragmas -Wno-comment
    OBJDUMP  := objdump
endif

LDFLAGS ?= -L./lib/

# ─── Link-Time Optimization (LTO) Type Detection ───────────────────────────────

# Automatically select LTO type based on compiler
ifeq ($(CXX),clang++)
    LTO_FLAG := -flto=thin        # Clang: thin LTO is more compatible and faster
else ifeq ($(CXX),clang)
    LTO_FLAG := -flto=thin
else ifneq ($(findstring clang,$(CXX)),)
    LTO_FLAG := -flto=thin        # Any clang-like compiler
else
    LTO_FLAG := -flto=auto        # GCC: use auto LTO for better optimization
endif

# ─── Optimization Flags ───────────────────────────────────────────────────────

ifeq ($(BUILD_TYPE),release)
    OPTFLAGS := $(OPT_RELEASE) -DNDEBUG
	ifneq ($(ARCH),)
        OPTFLAGS += -march=$(ARCH)
    endif
    ifeq ($(USE_LTO),true)
    ifneq ($(LTO_FLAG),)
        OPTFLAGS += $(LTO_FLAG)
        LDFLAGS  += $(LTO_FLAG)
    endif
    endif
else ifeq ($(BUILD_TYPE),debug)
    OPTFLAGS := $(OPT_DEBUG) -ggdb3 -DDEBUG -D_GLIBCXX_DEBUG
else ifeq ($(BUILD_TYPE),relwithdebinfo)
    OPTFLAGS := -O2 -ggdb3 -DNDEBUG -DRELWITHDEBINFO
else
    OPTFLAGS := -O0 -ggdb3 -DDEBUG
endif

# ─── Platform detection ─────────────────────────────────────────────────────────

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

	ifeq ($(USE_CONSOLE),false)
        CONSOLEFLAGS := -mwindows
    endif

	IS_WINDOWS := yes
	OS_NAME    := Windows
	SED        := sed -i
    
    HOST_ARCH ?= $(shell powershell -NoProfile -Command "[System.Environment]::GetEnvironmentVariable('PROCESSOR_ARCHITECTURE')")
    ifeq ($(HOST_ARCH),AMD64)
        HOST_ARCH := x86_64
    endif

else ifeq ($(UNAME_S),Darwin)
    IS_MACOS  := yes
	OS_NAME   := macOS
    RM        := rm -rf
    MKDIR     := mkdir -p
    FIXPATH   = $1
	SED       := sed -i ''
	HOST_ARCH ?= $(shell uname -m)

else ifeq ($(UNAME_S),Linux)
    IS_LINUX  := yes
	OS_NAME   := Linux
    RM        := rm -rf
    MKDIR     := mkdir -p
    FIXPATH   = $1
	SED       := sed -i
	HOST_ARCH ?= $(shell uname -m)
endif

TARGET_ARCH := $(if $(filter native,$(ARCH)),$(HOST_ARCH),$(ARCH))
TARGET := $(APP_NAME)-$(TARGET_ARCH)$(if $(filter Windows,$(OS_NAME)),.exe,)

# ─── Sanitizers & Analysis ────────────────────────────────────────────────────

SANITIZE_FLAGS :=
ifeq ($(BUILD_TYPE),debug)
	ifeq ($(UNAME_S),Linux)
		SANITIZE_FLAGS += -fsanitize=address -fsanitize=undefined
		SANITIZE_FLAGS += -fsanitize-recover=all
	endif

	ifeq ($(IS_MACOS),yes)
		SANITIZE_FLAGS := -fsanitize=address -fsanitize=undefined
	endif
endif

ANALYZE_FLAGS :=
ifeq ($(ANALYZE),true)
    ANALYZE_FLAGS += -fanalyzer -Wanalyzer-null-dereference
    CXXFLAGS += $(ANALYZE_FLAGS)
endif

# ─── Verbose ──────────────────────────────────────────────────────────────────

# 0 = Silent Mode; 1 = Color & Details Mode
VERBOSE ?= 1

# Optional: if the user passes -s (--silent), force VERBOSE=0
ifneq ($(findstring s,$(MAKEFLAGS)),)
    VERBOSE := 0
else ifneq ($(filter -j%,$(MAKEFLAGS)),)
	VERBOSE := 0
endif

# ─── Color Codes ──────────────────────────────────────────────────────────────

ifeq ($(VERBOSE),1)
	OK_COLOR     := \033[0;32m
	ERROR_COLOR  := \033[0;31m
	WARN_COLOR   := \033[0;33m
	VIOLET_COLOR := \033[0;35m
	INFO_COLOR   := \033[0;34m
	HIGHLIGHT    := \033[1;36m
	BOLD         := \033[1m
	NO_COLOR     := \033[0m
	
	TITLE_COLOR   := \033[1;35m
	LINES_COLOR   := \033[1;30m
else
	OK_COLOR      := 
	ERROR_COLOR   := 
	WARN_COLOR    := 
	VIOLET_COLOR  := 
	INFO_COLOR    := 
	HIGHLIGHT     := 
	BOLD          := 
	NO_COLOR      := 

	TITLE_COLOR   := 
	LINES_COLOR   := 
endif

# ─── Libraries ───────────────────────────────────────────────────────────────────

# Libraries to link against (use -l)
LIBS ?= 

ifeq ($(IS_WINDOWS),yes)
    # OpenGL on Windows
	LIBS += -lgdi32 -lopengl32
	
    # If using GUI app (no console), link additional libraries
    #LIBS += -luser32 -lgdi32 -lkernel32
	
    # Optional: link with Shell32 and Dwmapi for advanced Windows features
    # LIBS += -lshell32 -ldwmapi
endif

ifeq ($(IS_LINUX),yes)
    # OpenGL + X11 stack
    LIBS += -lGL -ldl -lpthread 
    LIBS += -lXrandr -lXinerama -lXcursor -lXi
endif

ifeq ($(IS_MACOS),yes)
    # macOS frameworks (use -framework instead of -l)
    LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	LIBS += -L/usr/local/lib -L/opt/local/lib -L/opt/homebrew/lib

	CXXFLAGS += -I/usr/local/include -I/opt/local/include -I/opt/homebrew/include
endif

# Final linker command ingredients
LDFLAGS += $(LIBS) $(CONSOLEFLAGS)

# ─── Recursive Source Discovery ───────────────────────────────────────────────

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

# ─── Phony Targets ────────────────────────────────────────────────────────────

.PHONY: all dirs debug release relwithdebinfo analyze docs asm disassemble \
        benchmark run run-debug clean clean-all \
        clean-docs clean-bench help info deps graph-deps

# ─── Build rules ──────────────────────────────────────────────────────────────

ifeq ($(words $(SOURCES)),0)
all: clean-banner
	@printf "\n$(WARN_COLOR)No source files found. Nothing to build.$(NO_COLOR)\n"
	@printf "  $(WARN_COLOR)Please add source files to the $(SOURCE_DIRS) directories.$(NO_COLOR)\n\n"
	@printf "Searched in: %s\n" "$(SOURCE_DIRS)"
	@exit 0
else
all: clean-banner dirs $(BIN_DIR)/$(TARGET)
endif

dirs:
	@$(MKDIR) "$(call FIXPATH,$(BIN_DIR))" 2>/dev/null || true
	@$(MKDIR) "$(call FIXPATH,$(OBJ_DIR))" 2>/dev/null || true
	@$(MKDIR) "$(call FIXPATH,$(DEP_DIR))" 2>/dev/null || true
	@$(MKDIR) "$(call FIXPATH,$(ASM_DIR))" 2>/dev/null || true
	@$(MKDIR) "$(call FIXPATH,$(DOC_BUILD))" 2>/dev/null || true
	@$(MKDIR) "$(call FIXPATH,$(BENCH_DIR))" 2>/dev/null || true
	
# ─── Primary Build Rules ──────────────────────────────────────────────────────

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@printf "\n$(LINES_COLOR)───────$(NO_COLOR) $(TITLE_COLOR)Linking$(NO_COLOR)\n"
	@printf "  %-14s : %s\n" "Target" "$(TARGET)"
	@printf "  %-14s : %s object(s)\n" "Objects" "$(words $(OBJECTS))"
	@$(CXX) $(OPTFLAGS) $(SANITIZE_FLAGS) $^ -o $@ $(LDFLAGS) \
		&& printf "  $(OK_COLOR)✓$(NO_COLOR) %-12s : $(OK_COLOR)%s$(NO_COLOR)\n" "Status" "Success" \
		|| printf "  $(ERROR_COLOR)✗$(NO_COLOR) %-12s : $(ERROR_COLOR)%s$(NO_COLOR)\n" "Status" "FAILED"
	@printf "$(LINES_COLOR)────────────────────────────────────────────$(NO_COLOR)\n"

$(OBJ_DIR)/%.o : %.$(SRC_EXT)
ifeq ($(VERBOSE),1)
	@printf "  $(OK_COLOR)[✓]$(NO_COLOR) $(OK_COLOR)Compiling$(NO_COLOR)  %-40s " "$<"
else
	@printf "[Compiling] %s\n" "$<"
endif
	@$(MKDIR) "$(@D)" >/dev/null 2>&1
	@$(MKDIR) "$(call FIXPATH,$(dir $(DEP_DIR)/$<))" >/dev/null 2>&1
	@$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(SANITIZE_FLAGS) $(INCLUDES) $(DEPFLAGS) -c $< -o $@ && \
		{ if [ "$(VERBOSE)" = "1" ]; then printf "$(OK_COLOR)[OK]$(NO_COLOR)\n"; fi; } || \
		{ if [ "$(VERBOSE)" = "1" ]; then \
			printf "$(ERROR_COLOR)[FAILED]$(NO_COLOR)\n"; \
			printf "  $(ERROR_COLOR)✗ Compilation failed for $<$(NO_COLOR)\n"; \
		else \
			printf "[ERROR] On compilation %s\n" "$<"; \
		fi; exit 1; }

# Only include existing dependency files - avoids infinite loops
-include $(wildcard $(DEPENDENCIES))

# ─── Build variants ──────────────────────────────────────────────────────────────

debug: BUILD_TYPE=debug
debug: clean-banner all
	@printf "\n$(OK_COLOR)✓ Debug build complete$(NO_COLOR)\n"
	@printf "$(WARN_COLOR)Note: Debug builds are not optimized and may run slower than release builds.$(NO_COLOR)\n"

release: BUILD_TYPE=release
release: clean-banner all
	@printf "\n$(OK_COLOR)✓ Release build complete$(NO_COLOR)\n"
	@printf "$(WARN_COLOR)Note: Release builds are optimized for performance and may be harder to debug.$(NO_COLOR)\n"


relwithdebinfo: BUILD_TYPE=relwithdebinfo
relwithdebinfo: clean-banner all
	@printf "\n$(OK_COLOR)✓ Release with debug info build complete$(NO_COLOR)\n"
	@printf "$(WARN_COLOR)Note: This build type includes optimizations and debug symbols, making it suitable for profiling and debugging optimized code.$(NO_COLOR)\n"
	
analyze: ANALYZE=true
analyze: USE_LTO=false
analyze: BUILD_TYPE=debug
analyze: OPT_DEBUG=-O0
analyze: clean-banner all
	@printf "\n$(WARN_COLOR)⚠ Static analysis enabled - Review output above$(NO_COLOR)\n\n"

# ─── Documentation Generation ─────────────────────────────────────────────────

DOXYGEN_CONFIG ?= Doxyfile
HAS_DOXYGEN    := $(shell command -v doxygen 2>/dev/null)

# ─── Documentation ────────────────────────────────────────────────────────────

docs: $(DOC_BUILD)/html/index.html
	@printf "$(OK_COLOR)✓ Documentation generated$(NO_COLOR)\n"

$(DOC_BUILD)/html/index.html: $(DOXYGEN_CONFIG) | dirs
ifdef HAS_DOXYGEN
	@printf "$(INFO_COLOR)[Doxygen]$(NO_COLOR) Generating documentation...\n"
	@doxygen $(DOXYGEN_CONFIG)
else
	@printf "$(ERROR_COLOR)✗ Doxygen not found$(NO_COLOR)\n"
	@exit 1
endif

$(DOXYGEN_CONFIG):
	@printf "$(INFO_COLOR)[Doxygen]$(NO_COLOR) Creating default Doxyfile...\n"
	@$(MKDIR) "$(call FIXPATH,$(DOC_BUILD))" 2>/dev/null || true
	@doxygen -g $(DOXYGEN_CONFIG)

	@$(SED) 's|^OUTPUT_DIRECTORY .*|OUTPUT_DIRECTORY = $(DOC_BUILD)|' $(DOXYGEN_CONFIG)
	@$(SED) 's|^INPUT .*|INPUT = $(SOURCE_DIRS)|' $(DOXYGEN_CONFIG)
	@$(SED) 's|^RECURSIVE .*|RECURSIVE = YES|' $(DOXYGEN_CONFIG)
	@$(SED) 's|^GENERATE_LATEX .*|GENERATE_LATEX = NO|' $(DOXYGEN_CONFIG)

# ─── Assembly & Disassembly ──────────────────────────────────────────────────────

asm: clean-banner dirs
	@printf "\n$(LINES_COLOR)───────$(NO_COLOR) $(TITLE_COLOR)Assembly$(NO_COLOR)\n"
	@printf "  %-12s : %s\n" "Sources" "$(words $(SOURCES))"
	@$(foreach src,$(SOURCES), \
		$(CXX) -S -masm=intel -O2 $(INCLUDES) -o $(ASM_DIR)/$(notdir $(basename $(src))).s $(src); \
	)
	@printf "  $(OK_COLOR)✓$(NO_COLOR) %-10s : %s\n" "Output" "$(ASM_DIR)"
	@printf "$(LINES_COLOR)────────────────────────────────────────────$(NO_COLOR)\n\n"
	
disassemble: clean-banner all
	@printf "\n$(LINES_COLOR)───────$(NO_COLOR) $(TITLE_COLOR)Disassembly$(NO_COLOR)\n"
	@printf "  %-12s : %s\n" "Target" "$(TARGET)"
	@$(OBJDUMP) -d -Mintel -C "$(call FIXPATH,$(BIN_DIR)/$(TARGET))" > "$(ASM_DIR)/$(APP_NAME)_disasm.txt"
	@$(OBJDUMP) -S -wC "$(call FIXPATH,$(BIN_DIR)/$(TARGET))" > "$(ASM_DIR)/$(APP_NAME)_source.txt"
	@printf "  $(OK_COLOR)✓$(NO_COLOR) %-10s : %s\n" "Output:" "$(ASM_DIR)"
	@printf "$(LINES_COLOR)────────────────────────────────────────────$(NO_COLOR)\n\n"

# ─── Benchmarking ─────────────────────────────────────────────────────────────

BENCHMARK_RUNS ?= 5

# List of benchmark variants to build and compare (can be overridden by user)
BENCH_VARIANTS ?= O0 O2 O3 O3-lto O3-native O3-lto-native

benchmark: BUILD_TYPE=release
benchmark: USE_LTO=true
benchmark: OPT_RELEASE=-O3
benchmark: clean-banner dirs
	@$(MAKE) --no-print-directory \
		BUILD_TYPE=release \
		USE_LTO=true \
		OPT_RELEASE=-O3 \
		ARCH=$(ARCH) \
		OBJ_DIR="$(BENCH_DIR)/obj" \
		DEP_DIR="$(BENCH_DIR)/dep" \
		BIN_DIR="$(BENCH_DIR)" \
		all
	@printf "\n$(LINES_COLOR)───────$(NO_COLOR) $(TITLE_COLOR)Benchmark (simple run)$(NO_COLOR)\n"
	@printf "  Binary:  $(BENCH_DIR)/$(TARGET)\n"
	@printf "  Runs:    $(BENCHMARK_RUNS)\n"
	@printf "  Arch:    $(TARGET_ARCH)\n\n"
	@for i in $$(seq 1 $(BENCHMARK_RUNS)); do \
		printf "  Run %2d: " "$$i"; \
		/usr/bin/time --format="real %e s   user %U s   sys %S s" \
			"$(BENCH_DIR)/$(TARGET)" 2>&1 | grep real; \
	done
	@printf "\n$(OK_COLOR)Done$(NO_COLOR)\n"

# ─── Run Rules ──────────────────────────────────────────────────────────────

run: release
	@printf "\n$(LINES_COLOR)───────$(NO_COLOR) $(TITLE_COLOR)Run$(NO_COLOR)\n"
	@printf "  %-12s : %s\n" "Target" "$(TARGET)"
	@printf "  $(INFO_COLOR)→ Running...$(NO_COLOR)\n"
	@$(BIN_DIR)/$(TARGET)
	@printf "$(LINES_COLOR)────────────────────────────────────────────$(NO_COLOR)\n\n"

run-debug: debug
	@printf "\n$(LINES_COLOR)───────$(NO_COLOR) $(TITLE_COLOR)Run Debug$(NO_COLOR)\n"
	@printf "  %-12s : %s\n" "Target" "$(TARGET)"
	@printf "  $(INFO_COLOR)→ Running DEBUG...$(NO_COLOR)\n"
	@$(BIN_DIR)/$(TARGET)
	@printf "$(LINES_COLOR)────────────────────────────────────────────$(NO_COLOR)\n\n"

# ─── Build UI variants ────────────────────────────────────────────────────────────

clean-banner:
	@if [ -t 1 ]; then \
		if command -v cls >/dev/null 2>&1; then cls; else clear; fi; \
	fi
ifeq ($(VERBOSE),1)
	@printf "$(LINES_COLOR)─── Build: $(TITLE_COLOR)$(APP_NAME)$(NO_COLOR)\n"
	@printf "  $(INFO_COLOR)Build Type:$(NO_COLOR)     $(BOLD)%s$(NO_COLOR)\n" "$(BUILD_TYPE)"
	@printf "  $(BOLD)> OS:$(NO_COLOR)           %s\n" "$(OS_NAME)"
	@printf "  $(BOLD)> Compiler:$(NO_COLOR)     %s ($(CXX_STD))\n" "$(CXX)"
	@printf "  $(BOLD)> Host Arch:$(NO_COLOR)    %s\n" "$(HOST_ARCH)"
	@printf "  $(BOLD)> Target Arch:$(NO_COLOR)  %s\n" "$(TARGET_ARCH)"
	@printf "$(LINES_COLOR)────────────────────────────────────────────$(NO_COLOR)\n"
else
	@printf "Building $(APP_NAME)\n"
	@printf "OS: $(OS_NAME)   |   Build: $(BUILD_TYPE)   |   TARGET arch: $(TARGET_ARCH)\n\n"
endif

# ─── Dependency Analysis ──────────────────────────────────────────────────────

deps:
	@printf "$(INFO_COLOR)[Dependencies]$(NO_COLOR)\n"
	@printf "  Sources: %d files\n" $(words $(SOURCES))
	@printf "  Objects: %d files\n" $(words $(OBJECTS))
	@printf "  Dependencies: %d files\n\n" $(words $(DEPENDENCIES))

graph-deps:
	@printf "$(INFO_COLOR)[Dependency Graph]$(NO_COLOR)\n"
	@for dep in $(DEPENDENCIES); do \
		echo "=== $$dep ===" && cat $$dep 2>/dev/null | head -5; \
	done | head -100

# ─── Clean Rules ──────────────────────────────────────────────────────────────

clean:
	@printf "$(LINES_COLOR)───────$(NO_COLOR) $(TITLE_COLOR)Clean$(NO_COLOR)\n"
	@printf "  %-12s : %s\n" "Removing" "OBJ, DEP, ASM, Binary"
	@$(RM) "$(call FIXPATH,$(OBJ_DIR))" "$(call FIXPATH,$(DEP_DIR))" "$(call FIXPATH,$(ASM_DIR))" "$(call FIXPATH,$(BIN_DIR)/$(TARGET))" "$(call FIXPATH,$(DOC_BUILD))" "$(call FIXPATH,$(BENCH_DIR))" 2>/dev/null || true
	@printf "  $(OK_COLOR)✓$(NO_COLOR) %-10s : %s\n" "Done" "$(OBJ_DIR) $(DEP_DIR) $(ASM_DIR) $(BIN_DIR) $(DOC_BUILD) $(BENCH_DIR)"
	@printf "$(LINES_COLOR)────────────────────────────────────────────$(NO_COLOR)\n\n"

clean-all:
	@printf "$(LINES_COLOR)───────$(NO_COLOR) $(TITLE_COLOR)Full Clean$(NO_COLOR)\n"
	@printf "  %-12s \n" " Removing all build artifacts..."
	@$(RM) "$(call FIXPATH,$(BUILD_BASE))" 2>/dev/null || true
	@printf "  $(OK_COLOR)✓$(NO_COLOR) %-10s : %s\n" "Done" "$(BUILD_BASE)"
	@printf "$(LINES_COLOR)────────────────────────────────────────────$(NO_COLOR)\n\n"

clean-docs:
	@printf "$(WARN_COLOR)[Cleaning]$(NO_COLOR) Removing documentation...\n"
	@$(RM) "$(call FIXPATH,$(DOC_BUILD))" 2>/dev/null || true
	@printf "  $(OK_COLOR)✓ Docs cleaned$(NO_COLOR)\n"

clean-bench:
	@$(RM) "$(call FIXPATH,$(BENCH_DIR))" 2>/dev/null || true
	@printf "$(WARN_COLOR)Benchmark directories cleaned$(NO_COLOR)\n"

# ─── Information & Help ────────────────────────────────────────────────────────

help:
	@printf "\n$(LINES_COLOR)───────$(NO_COLOR) $(TITLE_COLOR)Help$(NO_COLOR)\n"
	@printf "$(BOLD)Build Targets:$(NO_COLOR)\n"
	@printf "  $(OK_COLOR)all$(NO_COLOR)               - Build application (default)\n"
	@printf "  $(OK_COLOR)release$(NO_COLOR)           - Release build (-O3 -march=native -flto)\n"
	@printf "  $(OK_COLOR)debug$(NO_COLOR)             - Debug build with sanitizers\n"
	@printf "  $(OK_COLOR)sanitize$(NO_COLOR)          - Address & undefined behavior sanitizers\n"
	@printf "  $(OK_COLOR)analyze$(NO_COLOR)           - GCC static analysis\n"
	@printf "  $(OK_COLOR)relwithdebinfo$(NO_COLOR)    - Release with debug info\n\n"

	@printf "$(BOLD)Build Benchmarks:$(NO_COLOR)\n"
	@printf "  $(OK_COLOR)benchmark$(NO_COLOR)         - Run benchmarks with current settings\n"
	
	@printf "$(BOLD)Code Analysis:$(NO_COLOR)\n"
	@printf "  $(OK_COLOR)asm$(NO_COLOR)               - Generate assembly files\n"
	@printf "  $(OK_COLOR)disassemble$(NO_COLOR)       - Disassemble binary\n"
	@printf "  $(OK_COLOR)deps$(NO_COLOR)              - List source dependencies\n"
	@printf "  $(OK_COLOR)graph-deps$(NO_COLOR)        - Show dependency graph\n\n"

	@printf "$(BOLD)Documentation:$(NO_COLOR)\n"
	@printf "  $(OK_COLOR)docs$(NO_COLOR)              - Generate Doxygen documentation\n"
	@printf "  $(OK_COLOR)clean-docs$(NO_COLOR)        - Remove generated docs\n\n"

	@printf "$(BOLD)Maintenance:$(NO_COLOR)\n"
	@printf "  $(OK_COLOR)clean$(NO_COLOR)             - Remove object and binary files\n"
	@printf "  $(OK_COLOR)clean-all$(NO_COLOR)         - Remove all build artifacts\n"
	@printf "  $(OK_COLOR)clean-bench$(NO_COLOR)       - Remove benchmark builds\n\n"

	@printf "$(BOLD)Examples:$(NO_COLOR)\n"
	@printf "  make APP_NAME=myapp                    # Custom app name\n"
	@printf "  make debug                             # Debug with sanitizers\n"
	@printf "  make WARN_LEVEL=minimal                # Minimal warnings\n"
	@printf "  make WARN_LEVEL=normal ARCH=native     # Balanced warnings + optimization\n"
	@printf "  make ARCH=znver4 CXX=clang++           # AMD Zen4 with Clang\n"
	@printf "  make debug WARN_LEVEL=strict VERBOSE=1 # Full debug with detailed output\n"
	@printf "$(LINES_COLOR)────────────────────────────────────────────$(NO_COLOR)\n\n"

info:
	@printf "\n$(LINES_COLOR)───────$(NO_COLOR) $(TITLE_COLOR)Project Info$(NO_COLOR)\n"
	@printf "  %-20s : %s\n" "Project"       "$(APP_NAME)"
	@printf "  %-20s : %s file(s)\n" "Sources"       "$(words $(SOURCES))"
	@printf "  %-20s : %s\n" "Target"        "$(BIN_DIR)/$(TARGET)"
	@printf "  %-20s : %s\n" "Compiler"      "$(CXX)"
	@printf "  %-20s : %s\n" "C++ Standard"  "$(CXX_STD)"
	@printf "  %-20s : %s\n" "Build Type"    "$(BUILD_TYPE)"
	@printf "  %-20s : %s\n" "OS"            "$(OS_NAME)"
	@printf "  %-20s : %s\n" "Host Arch"     "$(HOST_ARCH)"
	@printf "  %-20s : %s\n" "Target Arch"   "$(TARGET_ARCH)"
	@printf "  %-20s : %s\n" "LTO"           "$(USE_LTO)"

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