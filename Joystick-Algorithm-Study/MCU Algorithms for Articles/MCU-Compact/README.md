# MCU-Compact — Joystick.h

**Platform:** Arduino Uno / Atmega328P / PlatformIO / C++
**Author:** Jesse Carpenter (carpentersoftware.com) — MageMCU
**Terminal Paths:** 9

---

## Description

This is the compact joystick-to-motor mapping algorithm ported from
`Unity-Compact/JoystickAlgorithm.cs` into the Arduino Uno C++ class structure.
It implements the same 9 terminal paths as MCU-Revised but uses cached absolute
values and nested ternary expressions to eliminate repeated `absT` calls,
producing cleaner and more efficient code.

Hardware testing on Arduino Uno with L298N motor driver is complete.
Output verified against MCU-Original across all 8 octants.
Note: Point 02 (X=0.0005, Y=0.0005) intentionally diverges — the tolerance
dead zone produces a clean stop here while MCU-Original falls into Octant 1.
This is the documented fix, not a discrepancy.

## Structure — 9 Terminal Paths

| # | Condition | Region |
|---|-----------|--------|
| 0 | absX < 0.001 && absY < 0.001 | STOP (Octant 0) |
| 1 | absX >= absY, X >= 0, Y >= 0 | Octant 1 |
| 2 | absX < absY,  X >= 0, Y >= 0 | Octant 2 |
| 3 | absX < absY,  X < 0,  Y >= 0 | Octant 3 |
| 4 | absX >= absY, X < 0,  Y >= 0 | Octant 4 |
| 5 | absX >= absY, X < 0,  Y < 0  | Octant 5 |
| 6 | absX < absY,  X < 0,  Y < 0  | Octant 6 |
| 7 | absX < absY,  X >= 0, Y < 0  | Octant 7 |
| 8 | absX >= absY, X >= 0, Y < 0  | Octant 8 |

## Key Improvements Over MCU-Revised

- `absX` and `absY` cached once — eliminates repeated `absT` calls per branch
- Delta simplified to `absT<real>(absX - absY)` using cached values
- Nested ternary expressions resolve two octants per branch (compact routing)
- Constructor explicitly initializes all member variables

## Delta Equation

```cpp
real absX = absT<real>(_inputX);
real absY = absT<real>(_inputY);
real delta = absT<real>(absX - absY);
```

Caching `absX` and `absY` first simplifies the delta expression compared to
the double `absT` form in MCU-Original and MCU-Revised, with identical results.

---

*Carpenter Software — carpentersoftware.com | MageMCU — github.com/MageMCU*
