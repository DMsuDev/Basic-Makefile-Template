# =============================================================================
# Makefile to build all examples in this project
# =============================================================================

# Default build type when running plain `make`
.DEFAULT_GOAL := all

# Platform-specific detection for commands

UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)

ifeq ($(findstring Windows,$(OS)),Windows)
    IS_WINDOWS := yes
else ifeq ($(UNAME_S),Darwin)
    IS_MACOS := yes
else ifeq ($(UNAME_S),Linux)
    IS_LINUX := yes
endif

# ─── Configuration ───────────────────────────────────────────────────────
EXAMPLES_DIR := examples

# ─── Common settings ─────────────────────────────────────────────────────
ifeq ($(OS),Windows_NT)
	RMDIR := rmdir /S /Q
else
	RMDIR := rm -rf
endif

# ─── Main rules ──────────────────────────────────────────────────────────
.PHONY: all donut-basic ImGui

all: donut-basic ImGui
	@echo "→ Built all examples"

donut-basic:
	@$(MAKE) --no-print-directory -C $(EXAMPLES_DIR)/donut-basic
	@echo "→ Built example: donut-basic"

ImGui:
	@$(MAKE) --no-print-directory -C $(EXAMPLES_DIR)/ImGui

# ─── Run targets ─────────────────────────────────────────────────────────
.PHONY: run-donut run-imgui

run-donut:
	@$(MAKE) --no-print-directory -C $(EXAMPLES_DIR)/donut-basic run

run-imgui:
	@$(MAKE) --no-print-directory -C $(EXAMPLES_DIR)/ImGui run

# ─── Clean targets ─────────────────────────────────────────────────────────
.PHONY: clean

clean:
	@$(MAKE) --no-print-directory -C $(EXAMPLES_DIR)/donut-basic clean
	@$(MAKE) --no-print-directory -C $(EXAMPLES_DIR)/ImGui clean
	@echo "→ Cleaned all examples"

# ─── Utilities ────────────────────────────────────────────────────────────
.PHONY: list help

list:
	@echo "Available examples:"
	@printf "  %-12s  (%s)\n" "imgui" "ImGui";
	@printf "  %-12s  (%s)\n" "donut" "donut-basic";

help:
	@echo "make .................... build all examples"
	@echo "make run-[alias] ........ build + run that example"
	@echo "                          (run-donut, run-imgui)"
	@echo "make clean .............. clean all examples"
	@echo "make list ............... show available examples"
	@echo "make help ............... this message"