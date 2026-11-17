# MCU-Revised — Joystick.h

**Platform:** Arduino Uno / Atmega328P / PlatformIO / C++
**Author:** Jesse Carpenter (carpentersoftware.com) — MageMCU
**Terminal Paths:** 9

---

## Article Update Note

This article copy now includes a code change in `Joystick.h`:

- Added `OutOctant()` to expose `_octant` as `int` (0–8)

Please update any related article text/snippets so published documentation matches the current code.

---

## Description

This is the revised joystick-to-motor mapping algorithm ported from
`Unity-Revised/Algorithm.cs` into the Arduino Uno C++ class structure.
It reduces the original 11 terminal paths to 9 by absorbing the
Forward/Backward and Left/Right axis-aligned cases into the octant hierarchy,
and replaces floating-point equality checks with a tolerance-based center stop.

## Structure — 9 Terminal Paths

| # | Condition | Region |
|---|-----------|--------|
| 0 | \|X\| < 0.001 && \|Y\| < 0.001 | STOP (Octant 0) |
| 1 | \|X\| >= \|Y\|, X >= 0, Y >= 0 | Octant 1 |
| 2 | \|X\| < \|Y\|,  X >= 0, Y >= 0 | Octant 2 |
| 3 | \|X\| < \|Y\|,  X <= 0, Y >= 0 | Octant 3 |
| 4 | \|X\| >= \|Y\|, X <= 0, Y >= 0 | Octant 4 |
| 5 | \|X\| >= \|Y\|, X <= 0, Y <= 0 | Octant 5 |
| 6 | \|X\| < \|Y\|,  X <= 0, Y <= 0 | Octant 6 |
| 7 | \|X\| < \|Y\|,  X >= 0, Y <= 0 | Octant 7 |
| 8 | \|X\| >= \|Y\|, X >= 0, Y <= 0 | Octant 8 |

## Key Improvements Over MCU-Original

- Tolerance-based center stop replaces floating-point equality checks
- No floating-point equality comparisons anywhere in the logic
- Forward/Backward and Left/Right absorbed into octant branches
- Constructor explicitly initializes all member variables
- `absT<real>` used consistently — no silent float truncation
- Octant number tracked in `_octant` member (0–8)
- `OutOctant()` getter returns `_octant` as `int` (0–8)

## What `_octant` Is Used For

`_octant` is the integer region label for the current joystick state.

- **0** = stop (inside dead-zone tolerance)
- **1-8** = directional octants used by the mapping branches

Exposing it with `OutOctant()` supports:

- integer-based conditional routing in control code
- kinematics/mode selection logic
- QA verification and runtime telemetry by octant number

## Delta Equation

```cpp
real delta = absT<real>(absT<real>(_inputX) - absT<real>(_inputY));
```

The double `absT` wrap is intentional and load-bearing — same reasoning as
MCU-Original. Note: the operand order is X−Y here (vs Y−X in MCU-Original),
which is mathematically equivalent since the outer absT makes the sign irrelevant.

---

*Carpenter Software — carpentersoftware.com | MageMCU — github.com/MageMCU*
