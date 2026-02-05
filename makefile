# =============================================================================
# Makefile to build all examples in this project
# =============================================================================

# Default build type when running plain `make`
.DEFAULT_GOAL := all

# Ensure consistent behavior across make versions
.SUFFIXES:

# Platform-specific detection for commands

UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)

ifeq ($(findstring Windows,$(OS)),Windows)
    IS_WINDOWS := yes
    RMDIR := rmdir /S /Q
else ifeq ($(UNAME_S),Darwin)
    IS_MACOS := yes
    RMDIR := rm -rf
else
    IS_LINUX := yes
    RMDIR := rm -rf
endif

# ─── Configuration ───────────────────────────────────────────────────────
EXAMPLES_DIR := examples

# ─── Main rules ──────────────────────────────────────────────────────────
.PHONY: all donut-basic ImGui run-donut run-imgui clean clean-all list help

all: donut-basic ImGui
	@echo "→ Built all examples"

donut-basic:
	@$(MAKE) --no-print-directory -C $(EXAMPLES_DIR)/donut-basic
	@echo "→ Built example: donut-basic"

ImGui:
	@$(MAKE) --no-print-directory -C $(EXAMPLES_DIR)/ImGui
	@echo "→ Built example: ImGui"

# ─── Run targets ─────────────────────────────────────────────────────────
run-donut:
	@$(MAKE) --no-print-directory -C $(EXAMPLES_DIR)/donut-basic run

run-imgui:
	@$(MAKE) --no-print-directory -C $(EXAMPLES_DIR)/ImGui run

# ─── Clean targets ─────────────────────────────────────────────────────────
clean:
	@$(MAKE) --no-print-directory -C $(EXAMPLES_DIR)/donut-basic clean
	@$(MAKE) --no-print-directory -C $(EXAMPLES_DIR)/ImGui clean
	@echo "→ Cleaned all examples"

clean-all:
	@$(RMDIR) "$(EXAMPLES_DIR)/donut-basic/build" 2>/dev/null || true
	@$(RMDIR) "$(EXAMPLES_DIR)/ImGui/build" 2>/dev/null || true
	@echo "→ Removed all build directories"

# ─── Utilities ────────────────────────────────────────────────────────────
list:
	@printf "\n═══════ Available Examples ═══════\n"
	@printf "  %-15s  %-30s\n" "Alias" "Description"
	@printf "  %-15s  %-30s\n" "donut" "3D ASCII Donut (Terminal)"
	@printf "  %-15s  %-30s\n" "imgui" "ImGui + GLFW (GUI)"
	@printf "\n"

help:
	@printf "\n═══════ Example Build System ═══════\n"
	@printf "Usage: make [target]\n\n"
	@printf "Main Targets:\n"
	@printf "  all            build all examples (default)\n"
	@printf "  donut-basic    build donut example only\n"
	@printf "  ImGui          build imgui example only\n\n"
	@printf "Run Targets:\n"
	@printf "  run-donut      build + run donut example\n"
	@printf "  run-imgui      build + run imgui example\n\n"
	@printf "Clean Targets:\n"
	@printf "  clean          clean all examples\n"
	@printf "  clean-all      remove all build directories\n\n"
	@printf "Utilities:\n"
	@printf "  list           show available examples\n"
	@printf "  help           this message\n\n"