# Modern C++ Makefile Template

[![Make](https://img.shields.io/badge/Build-Make-6D00CC?style=flat&logo=cmake&logoColor=white)](https://www.gnu.org/software/make/)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey?style=flat)
[![C](https://img.shields.io/badge/Language-C-A8B9CC?style=flat&logo=c)](<https://en.wikipedia.org/wiki/C_(programming_language)>)
[![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=flat&logo=cplusplus&logoColor=white)](https://isocpp.org/)
![Status](https://img.shields.io/badge/Status-Experimental-blueviolet?style=flat)

[![MSVC](https://img.shields.io/badge/Build-MSVC-5C2D91?style=flat)](https://learn.microsoft.com/en-us/cpp/)
[![Clang](https://img.shields.io/badge/Build-Clang-262D3A?style=flat&logo=llvm&logoColor=white)](https://clang.llvm.org/)
[![GCC](https://img.shields.io/badge/Build-GCC-000000?style=flat&logo=gnu&logoColor=white)](https://gcc.gnu.org/)

A clean, lightweight, and cross-platform **Makefile** for C++ projects.  
Supports Windows (**MinGW-w64/MSYS2**), **Linux**, and **macOS** with minimal configuration.

Designed to be simple, readable, and easy to extend — ideal for game engines, tools, small-to-medium applications, learning projects, or as a starting point.

![Demo](.github/readme/demo.gif)

## Features

- Debug & Release configurations
- Architecture-specific optimization (`-march=native` by default)
- Dependency tracking with `.d` files
- Assembly generation (`make asm`) and binary disassembly (`make disasm`)
- Optional console hiding on Windows (`USE_CONSOLE=false`)
- Multi-core parallel builds support with clean handling

# Main Commands

| Command           | Description                               | When to use                           |
| ----------------- | ----------------------------------------- | ------------------------------------- |
| `make`            | Release build (default target)            | Everyday development                  |
| `make release`    | Explicit Release build with optimizations | Final/performance builds              |
| `make debug`      | Debug build + symbols + sanitizers        | Bug hunting, ASan/UBSan               |
| `make run`        | Release build + execute binary            | Quick testing                         |
| `make run-debug`  | Debug build + execute binary              | Debugging sessions                    |
| `make asm`        | Generate Intel-syntax `.s` assembly files | Inspecting compiler output            |
| `make disasm`     | Disassemble final binary (objdump)        | Optimization / reverse engineering    |
| `make clean`      | Remove objects, deps, asm, binary         | Fresh start for current config        |
| `make full-clean` | Delete entire `./build/` directory        | Changing compiler or major flags      |
| `make help`       | Show help message                         | Quick reference                       |
| `make info`       | Show project configuration summary        | Verify paths, compiler, sources count |

## Parallel / Multi-Core Builds (Recommended for Speed)

The **Makefile** fully supports parallel compilation to use multiple **CPU cores** and dramatically reduce build times on modern machines.

### How to Use It

```bash
# Use all available cores (recommended)
make -j$(nproc) run     # Linux/WSL/macOS
make -j8 run            # Windows or fixed number (adjust to your CPU)

# Example: build with 8 cores
make -j8

# Or with run
make run -j12
```

- **Automatic behavior**: When you use `-j` (parallel mode), the **Makefile** automatically reduces verbosity to avoid chaotic interleaved output.
  - No fancy colors or spinners per file
  - Only essential messages and errors are shown
  - This prevents the terminal from becoming a mess when compiling dozens/hundreds of files at once.

- **Tip**: Start with `-j4` or `-j8` and increase until you find the sweet spot for your machine (too high can cause memory thrashing if RAM is limited).
- **Pro tip**: Combine with silent mode for even cleaner logs in CI:

```Bash
make -j$(nproc) -s > build.log 2>&1
```

## Customization

### 1. Basic project settings

```makefile
APP_NAME     ?= MyGame                  # Your project name
SOURCE_DIRS  ?= src src/core src/utils  # Add folders as needed
INCLUDE_DIRS ?= include                 # Header search paths
LANGUAGE     ?= c++23                   # c++20, c++26, gnu++23...
USE_CONSOLE  ?= true                    # Set to false for GUI apps on Windows
```

## 1. Compiler & architecture

```bash
# Use Clang instead of GCC
make CXX=clang++ release

# Optimize for specific CPU
make release ARCH=znver4          # AMD Zen 4
make release ARCH=skylake         # Intel 6th–9th gen
make release ARCH=armv8-a         # ARM (requires cross-compiler)
```

## 3. Adding libraries (example: GLFW + OpenGL)

Uncomment/add in the libraries section:

```makefile
ifeq ($(OS),Windows_NT)
    LIBS += -lglfw3dll -lopengl32 -lgdi32
endif

ifeq ($(OS_NAME),Linux)
    LIBS += -lglfw -lGL -ldl -lpthread
endif

ifeq ($(OS_NAME),macOS)
    LIBS += -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
endif
```

## 🗂️ Recommended Project Structure

```txt
MyProject/
├── src/
│   ├── main.cpp
│   ├── core/
│   │   └── engine.cpp
│   └── renderer/
├── include/
│   └── myproject/
├── lib/                  # optional: .a, .lib, .dll files
├── build/
│   ├── bin/              # final executable
│   ├── obj/              # object files
│   ├── dep/              # .d dependency files
│   └── asm/              # assembly & disassembly output
└── Makefile
```

## 📦 Requirements

- **Windows**: MinGW-w64 (MSYS2 recommend)
- **Linux / WSL**: GCC/Clang + dev packages (`sudo apt install libglfw3-dev libgl1-mesa-dev`)
- **Optional tools**:
  - `objdump` (for `make disasm`)
  - `gdb` / `lldb` (debugging)

## ⚠️ Troubleshooting

- **Linking errors** → Install missing dev packages (e.g., **libglfw3-dev** on **Ubuntu/WSL**)
- **No rule to make target** → Verify source files exist in `src/` (or added folders)
- **Sanitizers not working on Windows** → Disabled by design (partial support in **_MinGW_**)
- **Double slashes in paths** → Usually harmless; caused by empty variables in some shells
- **Sources not being detected** → Add source directories manually; `recursive discovery is not fully implemented yet`
- **Colors broken in CI** → Parallel mode auto-disables fancy output
- **Too much output with `-j`** → Use `-jN` `-s` or redirect to log

## 🐞 Known Issues / Limitations

This project has been primarily developed and tested on **Windows**.

Support for **macOS** and **Linux** is still experimental and may present unexpected behavior.

If you encounter any issues, platform-specific bugs, or inconsistencies, **your feedback is extremely valuable.**

## 🤝 Contributing

Contributions, issues and feature requests are welcome. </br>
Feel free to check the issues page.

## 📜 License

[![License](https://img.shields.io/badge/License-MIT-green?style=flat)](LICENSE)

Happy coding! ❤️
