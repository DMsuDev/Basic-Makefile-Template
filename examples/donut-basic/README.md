# Donut 3D Graphics - ASCII Art Renderer

A mesmerizing 3D donut rendering engine using only ASCII characters and trigonometry.

## Features

🎨 **Core Features**

- **Real-time 3D rotation**: Smooth spinning donut in ASCII
- **Mathematical accuracy**: Proper 3D projection using sine/cosine
- **Terminal rendering**: Works in any standard terminal
- **Cross-platform**: Windows, Linux, macOS support
- **Ultra-lightweight**: No graphics libraries required

## How It Works

The donut renderer uses:

1. **3D Geometry**: Torus (donut) surface equations
2. **Rotation matrices**: X, Y, Z axis rotations via trigonometry
3. **Perspective projection**: Projects 3D points onto 2D terminal
4. **Z-buffering**: Simple depth calculation for proper overlap
5. **ASCII mapping**: Characters mapped to luminance values

### Mathematical Foundation

The torus is defined parametrically:

```txt
x = (R + r*cos(v)) * cos(u)
y = r * sin(v)
z = (R + r*cos(v)) * sin(u)
```

Where:

- `R` = major radius (donut thickness)
- `r` = minor radius (ring thickness)
- `u, v` = parameters from 0 to 2π

## Building

### Requirements

- C++17+ compiler (GCC, Clang, MSVC)
- Standard Library only (no external dependencies)

### Compilation

```bash
cd examples/donut-basic

# Release build
make clean release

# Debug build
make debug

# Full clean
make clean-all
```

### Output

- **Binary**: `build/app/donut.exe` (Windows) or `build/app/donut` (Linux/macOS)

## Usage

Simply run the executable:

```bash
./donut
```

The donut will start rotating in your terminal. Press `Ctrl+C` to exit.

## Project Structure

```txt
donut-basic/
├── include/
│   └── donut.h         # Math & rendering functions
├── src/
│   ├── main.cpp        # Entry point & main loop
│   └── core/
│       └── donut.cpp   # 3D donut implementation
├── docs/
│   └── DONUT_EXPLAINED.md  # Detailed mathematical explanation
├── makefile            # Build configuration
└── README.md           # This file
```

## Algorithm Overview

### 1. Surface Generation

For each point (u, v) on the torus:

```cpp
float x = (R + r * cos(v)) * cos(u);
float y = r * sin(v);
float z = (R + r * cos(v)) * sin(u);
```

### 2. 3D Rotation

Apply rotation matrices to rotate the torus:

```cpp
// Rotation around X-axis
x' = x
y' = y*cos(angle) - z*sin(angle)
z' = y*sin(angle) + z*cos(angle)
```

### 3. Perspective Projection

Project 3D coordinates to 2D terminal:

```cpp
scale = 1 / (4 + z)  // Simple perspective
screenX = centerX + scale * x
screenY = centerY + scale * y
```

### 4. Z-Buffer & Rendering

Use Z-buffer to determine which character to display:

```cpp
if (z > buffer[x][y]) {
    buffer[x][y] = z;
    screen[x][y] = getCharForDepth(z);
}
```

## Performance

- **Frame rate**: 20-60 FPS depending on terminal size
- **Memory**: < 1 MB
- **CPU**: Minimal (pure math, no heavy graphics)
- **Compatibility**: Works over SSH, in Docker, on TTY

## Customization

Edit `include/donut.h` to modify:

- `R` - major radius (dough thickness)
- `r` - minor radius (ring thickness)
- Animation speed
- Terminal size
- Character set
- Rotation axes

## Build Targets

```bash
make all              # Build application
make release          # Optimized build
make debug            # Debug with symbols
make clean            # Remove artifacts
make help             # Show all targets
```

## Mathematical References

- **Torus equations**: [Wolfram MathWorld - Torus](https://mathworld.wolfram.com/Torus.html)
- **Rotation matrices**: [3D Rotation](https://en.wikipedia.org/wiki/Rotation_matrix)
- **Perspective projection**: [Computer Graphics](https://learnopengl.com/Getting-started/Coordinate-Systems)

## Platform Compatibility

| Platform | Status    |
| -------- | --------- |
| Windows  | ✅ Tested |
| Linux    | ✅ Tested |
| macOS    | ✅ Tested |
| SSH/TTY  | ✅ Full   |

## Fun Facts

- Uses **no graphics libraries** - pure math!
- Works over SSH from retro computers
- Terminal rendering makes it portable everywhere
- Great way to learn 3D graphics fundamentals
- Can run on ancient hardware

## License

MIT License - 2025/2026

## See Also

- [DONUT_EXPLAINED.md](docs/DONUT_EXPLAINED.md) - Deep dive into the mathematics
- [task-manager/](../task-manager/) - Another example project
