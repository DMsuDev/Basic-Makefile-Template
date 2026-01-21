# Donut ASCII — Detailed Explanation

This document explains how the rotating ASCII donut (torus) demo works. It covers the geometric model, rotation, projection and depth buffering, luminance (shading), runtime optimizations implemented in `src/core/donut.cpp`, and tuning notes.


Quick run (example):

```bash
./build/app/$(ProjectName).exe 100 40 0 30
```

Arguments: `width` `height` `frames(0=infinite)` `fps` — adjust to taste.


## 1 - Torus parameterization

A torus can be parameterized by two angles, $\theta$ (around the main ring) and $\phi$ (around the tube):

$$
\begin{aligned}
X(\theta,\phi) &= (R + r\cos\phi)\cos\theta \\
Y(\theta,\phi) &= (R + r\cos\phi)\sin\theta \\
Z(\theta,\phi) &= r\sin\phi
\end{aligned}
$$

In the code:

- `R2` represents the large radius (distance from torus center to tube center).
- The tube radius `r` is represented implicitly through the sampling and projection constants.

The renderer samples $\theta$ and $\phi$ at fixed steps to produce a grid of surface points. Trigonometric values for each sample are precomputed to avoid repeated sin/cos calls in the inner loop.

Code example (parameterization):

```cpp
// precomputed: cTheta, sTheta, cPhi, sPhi
float h = cPhi + cfg.R2;       // h = R + r * cos(phi)
float x3 = h * cTheta;         // X
float y3 = h * sTheta;         // Y
float z3 = sPhi;               // Z
```

In the code `h` is the radius from the torus center to the current tube point. Multiplying `h` by `cos(theta)` & `sin(theta)` gives the X/Y coordinates around the main ring; `sPhi` gives the tube's Z offset.

## 2 - Rotation

The torus is rotated in 3D using two angles named `A` and `B`. Instead of forming full rotation matrices, the code composes the rotation by combining sines and cosines of `A` and `B` with the parameterized coordinates. Updating `A` and `B` each frame produces the spinning animation.

Code example (rotation):

```cpp
float sinA = std::sin(A), cosA = std::cos(A);
float sinB = std::sin(B), cosB = std::cos(B);

// intermediate values used by the projection code:
float t = sTheta * h * cosA - sPhi * sinA;
// rotated X' and Y' are assembled below when computing screen x/y
```

Plain-English: `t` bundles parts of the rotated Y/Z contribution so final screen X/Y can be written compactly and the inner loop stays fast.

## 3 - Projection and depth buffering

The demo approximates perspective by computing an inverse depth factor:

$$
D = \frac{1}{\text{denom}} \approx \frac{1}{z + K}
$$

Here `denom` is a linear expression built from rotated coordinates and the constant `K2` (used to shift/scale depth and avoid division-by-zero). The projected screen coordinates are computed as scaled products of this depth factor and the rotated X/Y expressions.

To resolve occlusion the renderer maintains a z-buffer (`zbuffer`) with one float per screen cell. The code initializes the z-buffer to `-infinity`. For each projected sample the renderer computes `D` (an approximation of 1/z) and compares it to `zbuffer[index]`: if `D` is larger, that sample is closer and it overwrites both the z-buffer and the output character for that pixel.

Notes from the implementation:

- The code skips degenerate projections where the computed denominator equals zero.
- Larger `D` means closer to the camera because `D` is proportional to 1/z.

Code example (projection & depth):

```cpp
float denom = (sTheta * h * sinA + sPhi * cosA + cfg.K2);
if (denom == 0.0f) continue; // skip invalid projection
float D = 1.0f / denom; // approximate 1/z for depth

int x = static_cast<int>(width/2 + cfg.scale_x * D * (cTheta * h * cosB - t * sinB));
int y = static_cast<int>(height/2 + cfg.scale_y * D * (cTheta * h * sinB + t * cosB));
```

Code example (depth test):

```cpp
int o = x + width * y;
if (D > zbuffer[o]) {
    zbuffer[o] = D;                       // keep nearer depth
    output[o] = luminance[lum_index];     // write chosen ASCII char
}
```

Plain: `D` scales projected coordinates to mimic perspective; the z-buffer keeps the largest `D` per pixel (nearest sample).

## 4 - Luminance / shading

Shading uses a small palette of ASCII characters ordered by brightness: `.,-~:;=!*#$@`.

Luminance in the implementation is computed as a cheap dot-product-like scalar between an approximate surface normal and an implicit light direction. The code uses a compact expression combining sines/cosines of the sample angles and the rotation angles to produce a scalar `L`. That scalar is then scaled and converted to an index into the luminance string, with bounds clamping before indexing.

Implementation details (conceptual):

- Compute scalar luminance $L$ from surface orientation and light direction.
- Map $L$ into an integer index, e.g. `int idx = static_cast<int>(8.0f * L);` and clamp to `[0, lum_len-1]`.
- Use `luminance[idx]` to choose the ASCII character.

The exact expression used in `src/core/donut.cpp` is a compact approximation chosen for speed; it is not a precise normal·light dot product but produces visually plausible shading for the ASCII rendering.

Code example (luminance mapping):

```cpp
float L = /* compact expression combining sines/cosines and A/B */;
int lum_index = static_cast<int>(8.0f * L);
lum_index = std::clamp(lum_index, 0, lum_len - 1);
char ch = luminance[lum_index];
```

Plain: scaling `L` by 8 (an empirical choice matching the luminance palette size and visual contrast) converts continuous brightness into discrete ASCII levels; clamping prevents invalid indices.

## 5 - Optimizations implemented

- Trig precomputation: `precomputeTrig(...)` builds `sin`/`cos` lookup tables for `\theta` and `\phi` samples to avoid repeated expensive calls.
- Z-buffer: the depth buffer is filled with `-infinity` so any valid `D` will be greater and therefore overwrite it.
- Minimal diff rendering: the renderer keeps a `prev_output` frame and computes minimal horizontal runs of changed characters. Each run is emitted as a cursor-move ANSI sequence followed by the changed characters.
- Single I/O flush per frame: all ANSI cursor moves and changed runs are concatenated into one `diff_out` string and written once via `std::cout << diff_out << std::flush;` to reduce terminal I/O overhead and flicker.
- Stable FPS: frame timing uses `std::chrono::steady_clock` and sleeps only for the remaining time to match the target `fps`.

Code example (efficient terminal output):

```cpp
// move cursor to row/col and write run of changed characters
diff_out += "\x1b[" + std::to_string(row + 1) + ";" + std::to_string(start + 1) + "H";
diff_out.append(&output[row*width + start], run_len);
```

Plain: instead of redrawing the whole screen, the renderer writes only the changed horizontal runs, reducing I/O and flicker.

## 6 - Code structure

Key functions in `src/core/donut.cpp`:

- `precomputeTrig(theta_step, phi_step, ...)` — builds sin/cos lookup tables. It computes counts with `ceil(2*PI/step)` so the sample grids cover the full 0..2π range.
- `renderFrame(cfg, sinTheta, cosTheta, sinPhi, cosPhi, A, B, output, zbuffer)` — iterates sample points, computes projection, depth, luminance and updates `output`/`zbuffer`.
- `buildDiffAndUpdatePrev(output, prev_output, width, height)` — builds a compact ANSI-encoded diff and updates `prev_output`.
- `run(cfg)` — orchestrates initialization, the main loop, timing, and cursor hide/show sequences.

This separation keeps the computational kernel isolated from terminal I/O and makes parts easier to unit-test.

## 7 - Parameters and tuning

- `width`, `height`: terminal size (characters). Higher values increase detail at higher CPU cost.
- `theta_step`, `phi_step`: angular sampling resolution. Smaller steps produce smoother output but more computation.
- `scale_x`, `scale_y`: screen scale factors controlling torus size.
- `R2`, `K2`: geometry and projection constants; tweak to change shape and perspective.
- `fps`: target frames per second.

Try different runtime parameters to trade off quality and performance.

## 8 - Terminal compatibility

The demo uses ANSI escape sequences for cursor movement and cursor visibility. On Unix-like terminals these work by default. On Windows, enable ANSI (virtual terminal) processing or run inside a terminal that supports it (Windows Terminal, or recent cmd/PowerShell with `ENABLE_VIRTUAL_TERMINAL_PROCESSING` enabled). The code does not modify Windows console modes automatically to preserve portability.

---

### NOTE

This Donut demo and the refactor were created with assistance from an AI. The goal was to provide a clear, modular example that compiles with the repository's Makefiles and demonstrates minimal but effective ASCII 3D rendering.