# MCU-Original — Joystick.h

**Platform:** Arduino Uno / Atmega328P / PlatformIO / C++
**Repository:** [Joystick-UNO-L298N](https://github.com/MageMCU/Joystick-Uno-L298N)
**Author:** Jesse Carpenter (carpentersoftware.com) — MageMCU

---

## Purpose

This is the **original** C++ implementation of the joystick-to-motor mapping algorithm. It is a C++ template class `Algorithm<real>` inside the `csjc` namespace in the study copy (`MCU-Original/Joystick.h`). The production file (`Code - Joystick Uno L298N/include/Joystick.h`) uses the same logic with the class named `Joystick<real>`. It maps a two-axis joystick input to left and right motor output values for a differential drive robot connected through an L298N motor driver.

---

## How It Works

The algorithm divides the joystick input space into nine regions using a series of 11 conditionals:

- **8 octant branches** — one per directional region, organized by quadrant and axis dominance
- **2 axis-aligned branches** — separate cases for pure Forward/Backward (`inputX == 0`) and pure Left/Right (`inputY == 0`)
- **1 stop branch** — triggered when both axes equal exactly zero

The governing delta equation is:

```cpp
real delta = absT<float>(absT<float>(_inputY) - absT<float>(_inputX));
```

The double `absT` wrap is intentional. The inner wrap ensures both operands are non-negative magnitudes before subtraction; the outer wrap ensures the result is non-negative. Simplifying this equation fails in approximately three-quarters (~75%) of all cases when inputs carry opposite signs.

**Inputs:** `float inputX`, `float inputY` (normalized, range -1.0 to 1.0)
**Outputs:** `OutLeft()`, `OutRight()` (range -1.0 to 1.0)

---

## Known Issues

- **Floating-point equality checks** — The Forward/Backward, Left/Right, and STOP branches use `_inputX == (real)0` and `_inputY == (real)0`. Under real ADC noise conditions these comparisons will rarely evaluate true, making those branches effectively dead code on hardware.
- **No tolerance dead zone** — Without a dead zone, analog noise at the rest position can cause motor chatter.
- **Gaps and spikes** — Testing confirmed that under specific input conditions, particularly near octant boundaries, the algorithm can produce signal gaps (dead zones) and erratic output spikes. These are more pronounced under unit-circle normalized input but are present to a lesser degree under rectangular input as well.

These issues motivated the revised implementations documented in the sibling folders.

---

## Relationship to Other Files

| File | Relationship |
|------|-------------|
| `Unity-Revised/Algorithm.cs` | First C# rewrite — corrects the equality checks with a tolerance dead zone, restructures the 11 conditionals into a cleaner 9-branch hierarchy |
| `Unity-Compact/JoystickAlgorithm.cs` | Compact C# refactor — further optimizes Algorithm.cs; the logic here is the direct source for the MCU-Compact port |
| `MCU-Compact/Joystick.h` | Revised and hardware-validated C++ implementation — ports the compact C# logic back into this same class interface, fixing all known issues while keeping the public API identical |

The class interface (`UpdateInputs`, `OutLeft`, `OutRight`, `IsLeftForward`, `IsRightForward`) is preserved across both C++ versions, meaning `MCU-Compact/Joystick.h` is a drop-in replacement for this file with no changes required in `main.cpp`.

---

## Hardware Context

The physical joystick wired to the Atmega328P delivers voltage across a rectangular area. After `LinearMap<float>(0, 1023, -1.0, 1.0)` transformation, both axes independently span -1.0 to 1.0. Corner values such as (1.0, 1.0) are valid inputs. The algorithm was designed for this **rectangular input space**, not for unit-circle normalized gamepad input.

---

> **Status:** ⚠️ **ORIGINAL — DO NOT USE FOR NEW DEPLOYMENTS**
> 
> This file contains the original implementation with known issues:
> - Floating-point equality checks that fail under ADC noise
> - Gaps and spikes at octant boundaries under certain input conditions  
> - No tolerance dead zone for motor stability
> 
> Preserved here for reference and historical comparison.
> **For new deployments, use `MCU-Compact/Joystick.h`** (successfully hardware tested).

---

## Quality Assurance — 20-Point Test Results

Tested against the same 20-point input set used throughout this study (June 25, 2026).

**19 of 20 points agree with the three revised algorithms.**

**Point 02 (X=0.0005, Y=0.0005) — DIVERGES:**
MCU-Original has no tolerance dead zone. Both axes at 0.0005 are above exact zero,
so the algorithm falls into Octant 1 and outputs `Left=0.0005, Right=0.0000` instead
of a clean stop. Under real ADC noise conditions at the joystick rest position, the
motors receive a small non-zero drive signal rather than stopping cleanly. This is the
documented defect that motivated the revision effort.

All 19 remaining points produce output identical to the three revised algorithms.

*Full results: see `QA-Report.md` in the `Joystick-Algorithm-Study/` folder.*
