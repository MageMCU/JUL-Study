# MCU-Compact — Joystick.h

**Platform:** Arduino Uno / Atmega328P / PlatformIO / C++
**Repository:** [Joystick-UNO-L298N](https://github.com/MageMCU/Joystick-Uno-L298N)
**Author:** Jesse Carpenter (carpentersoftware.com) — MageMCU

---

## Purpose

This is the **revised C++ implementation** of `Joystick.h`, porting the compact octant algorithm from `Unity-Compact/JoystickAlgorithm.cs` back into the original C++ template class. It is a drop-in replacement for `MCU-Original/Joystick.h` — the class interface is unchanged (same namespace `csjc`, same template parameter `real`, same public methods) so existing `main.cpp` code requires **no modification**.

The only change is inside `_joystick()`, where the 11-conditional original is replaced by a 9-conditional compact core.

---

## How It Works

Three key structural improvements over `MCU-Original/Joystick.h`:

**1. Tolerance-based center stop**
```cpp
const real _tolerance = (real)0.001;
if (absX < _tolerance && absY < _tolerance) { /* stop */ }
```
Replaces the three floating-point equality checks (`== (real)0`) for STOP, Forward/Backward, and Left/Right — checks that fail silently under real ADC noise conditions.

**2. Cached absolute values**
```cpp
real absX = absT<real>(_inputX);
real absY = absT<real>(_inputY);
real delta = absT<real>(absX - absY);
```
Eliminates repeated `absT` calls on every branch comparison. Caching once up front mirrors the optimization in `JoystickAlgorithm.cs`.

**3. Unified octant hierarchy (9 conditionals vs. 11)**
The Forward/Backward and Left/Right axis-aligned special cases are absorbed into the unified octant branches. The result is a cleaner, more predictable conditional structure.

**Inputs:** `float inputX`, `float inputY` (normalized, range -1.0 to 1.0)
**Outputs:** `OutLeft()`, `OutRight()` (range -1.0 to 1.0)

---

## Public API (Unchanged)

```cpp
void UpdateInputs(real inputX, real inputY)  // Feed joystick values
real OutLeft()                               // Left motor output
real OutRight()                              // Right motor output
bool IsLeftForward()                         // True if left motor drives forward
bool IsRightForward()                        // True if right motor drives forward
```

The constructor now explicitly initializes all member variables to zero — the original constructor body was empty.

---

## Hardware Context

The physical joystick wired to the Atmega328P delivers voltage across a rectangular area. After `LinearMap<float>(0, 1023, -1.0, 1.0)` transformation, both axes independently span -1.0 to 1.0. The full input pipeline from `main.cpp` is:

1. Raw ADC values (0–1023) read from analog pins A0 and A1
2. Mapped to -1.0 to 1.0 via `LinearMap`
3. Centering offsets applied (`X_OFFSET = 0.05`, `Y_OFFSET = 0.06`)
4. Values fed into `UpdateInputs(inputX, inputY)`
5. Outputs mapped via `LinearMap<float>(-1.0, 1.0, -255, 255)` for the L298N motor driver

---

## Relationship to Other Files

| File | Relationship |
|------|-------------|
| `MCU-Original/Joystick.h` | The original file this revision replaces — same public API, corrected internals |
| `Unity-Revised/Algorithm.cs` | First C# revision — the conceptual predecessor, introduced the tolerance stop and 9-branch structure |
| `Unity-Compact/JoystickAlgorithm.cs` | **Direct source** — the compact C# logic was ported into this file's `_joystick()` method |

The chain of derivation is:
```
MCU-Original/Joystick.h
        ↓  (corrected, restructured)
Unity-Revised/Algorithm.cs
        ↓  (compact refactor)
Unity-Compact/JoystickAlgorithm.cs
        ↓  (ported back to C++)
MCU-Compact/Joystick.h
```

---

> **Status:** ✅ **Hardware Testing Complete**
>
> The revised `Joystick.h` has been successfully tested on Arduino Uno hardware with the L298N motor driver. All functionality works as expected across all joystick positions and motion types. The tolerance-based dead zone effectively eliminates ADC noise without introducing lag, and motor control is smooth and responsive throughout the full input range. See the QA-Report.md for detailed testing results.

---

## Quality Assurance — 20-Point Test Results

Tested against the same 20-point input set used throughout this study (June 25, 2026).

**20 of 20 points pass.**

Outputs are identical to `Unity-Revised/Algorithm.cs` and `Unity-Compact/JoystickAlgorithm.cs`
across all 20 test points — all 8 octants, center stop, tolerance boundary, and the
diagonal boundary where both axes are equal. No divergence found at any point.

Point 02 (X=0.0005, Y=0.0005) correctly returns `Left=0.0000, Right=0.0000` — the
tolerance dead zone produces a clean motor stop, confirming the C++ port faithfully
carries the fix from the Unity compact implementation.

*Full results: see `QA-Report.md` in the repository root.*
