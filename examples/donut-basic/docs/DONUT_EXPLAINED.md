# Donut ASCII — Detailed Explanation

This document explains the implementation details of the rotating ASCII donut (torus) demo included in this repository. It covers the mathematical model, projection, shading (luminance), depth buffering, and runtime optimizations used by the code in `src/core/donut.cpp`.

---

## 1 — Torus Parameterization

A torus (donut) can be parameterized by two angles, $\theta$ and $\phi$:

$$
\begin{aligned}
X(\theta,\phi) &= (R + r\cos\phi)\cos\theta \\
Y(\theta,\phi) &= (R + r\cos\phi)\sin\theta \\
Z(\theta,\phi) &= r\sin\phi
\end{aligned}
$$

Where:

- $R$ is the distance from the center of the tube to the center of the torus (`R2` in code).
- $r$ is the tube radius (we use an implicit scaling via steps and projection constants).

The demo uses a simplified projection and combines the parameters into scaled screen coordinates.

## 2 — Rotation

Two rotation angles (named `A` and `B` in the code) rotate the torus in 3D before projection. Rotation matrices are applied implicitly by computing the rotated coordinates via trigonometric combinations of $\sin$/$\cos$ of `A` and `B` and the torus parameters.

Rotation is applied to simulate the torus spinning in 3D space so the lighting and silhouette change over time.

## 3 — Projection & Depth (Z-buffer)

The demo approximates perspective by computing a depth factor $D$ as an inverse of an expression that depends on the rotated coordinates:

$$
D \approx \frac{1}{z + K}
$$

Where $K$ is a constant offset (`K2` in code) chosen to keep denominators positive and to control perceived scale. The projected screen coordinates are then:

$$
\begin{aligned}
x &= \text{screen\_cx} + S_x \cdot D \cdot X' \\
y &= \text{screen\_cy} + S_y \cdot D \cdot Y'
\end{aligned}
$$

($X'$, $Y'$ are rotated/translated partial expressions; $S_x$, $S_y$ are scale factors set in the `Config`.)

To correctly display surfaces in front of others we maintain a z-buffer (`zbuffer`) of size `width*height`. For each projected point we compare its $D$ (1/z) with the stored value: if $D` is larger, the pixel is closer and we update both the z-buffer and the output character.

## 4 — Luminance / Shading

The ASCII shading is chosen from a small palette of characters ordered by perceived brightness: `".,-~:;=!*#$@"`.

A scalar luminance value $L$ is approximated using a combination of surface normal direction and a hard-coded light direction (implicitly in the formula). The code maps $L$ into an index into the luminance array and clamps it to avoid out-of-range access.

Example (in code):

```cpp
float L = cPhi * cTheta * sinB - D * (sPhi * cosB + sTheta * cosA * sinB);
int index = clamp(lround(8.0f * L), 0, lum_len - 1);
char ch = luminance[index];
```

This is a cheap approximation but visually convincing for ASCII shading.

## 5 — Optimizations Implemented

The demo includes several important optimizations to improve runtime and reduce console I/O:

- **Trig precomputation:** compute `sin`/`cos` tables for the theta and phi sample grids once per run. This avoids expensive calls inside the inner loop.
- **Z-buffer (depth buffer):** store `-infinity` as initial values and compare `D` (1/z). Using `-infinity` ensures any valid `D` will be larger and write to the buffer.
- **Double-buffer diffing:** maintain a `prev_output` frame and compute minimal runs (horizontal segments) that changed, emitting only those runs as ANSI sequences. This reduces the number of characters written to the terminal and reduces flicker.
- **Single I/O write per diff:** each frame we build a single `diff_out` string containing cursor moves and changed runs, then write it once with `std::cout << diff_out << std::flush;`.
- **Stable FPS:** the code measures frame time with `std::chrono::steady_clock` and sleeps only for the remaining time to achieve a target FPS.

## 6 — Code Structure & Responsibilities (SOLID-inspired)

The implementation in `src/core/donut.cpp` was refactored into small functions, each having a single responsibility:

- `precomputeTrig(...)` — build lookup tables for sin/cos of `theta` and `phi`.
- `renderFrame(...)` — the pure computational part: iterate samples, compute projection, depth, and luminance; update `output` and `zbuffer` accordingly.
- `buildDiffAndUpdatePrev(...)` — compare `output` and `prev_output`, produce ANSI-encoded minimal updates and update `prev_output`.
- `run(...)` — orchestrates the loop: prepares buffers, calls helpers, manages timing, and handles cursor hide/show.

This separation makes unit testing easier (you can test `precomputeTrig` and `buildDiffAndUpdatePrev` independently) and keeps the core algorithm isolated from terminal I/O.

## 7 — Parameters & Tuning

- `width`, `height`: terminal dimensions; larger values increase visual fidelity but also computation.
- `theta_step` / `phi_step`: sampling density around the torus. Smaller steps produce smoother geometry at higher CPU cost.
- `scale_x` / `scale_y`: how large the torus appears on-screen.
- `R2` and `K2`: geometry/depth constants — tweak to change proportions and perspective.
- `fps`: frame rate target.

Try different values from the command line: `./build/app/MyProject-x86_64.exe 100 40 200 30` (width 100, height 40, 200 frames, 30 FPS).

## 8 — Terminal Compatibility

The demo uses ANSI escape sequences for cursor movement and hiding/showing cursor. On Unix terminals these work out-of-the-box. On Windows, enable ANSI processing (or run in a terminal that supports ANSI, e.g. Windows Terminal or recent cmd/powershell after enabling `ENABLE_VIRTUAL_TERMINAL_PROCESSING`). For automatic enabling on Windows, you can call the Win32 API to set console mode (not included to remain portable).

---

## Note on origin

This Donut demo and its supporting refactor were created with assistance from an AI. The goal was to demonstrate the Makefile-driven build and modular structure of the project.
