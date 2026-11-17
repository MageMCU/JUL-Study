# MCU-Original — Joystick.h

**Platform:** Arduino Uno / Atmega328P / PlatformIO / C++
**Author:** Jesse Carpenter (carpentersoftware.com) — MageMCU
**Terminal Paths:** 11

---

## Description

This is the original joystick-to-motor mapping algorithm as deployed in the
`Joystick-UNO-L298N` repository. The input space is divided using four quadrant
checks, each containing two octant branches, plus three axis-aligned cases.

## Structure — 11 Terminal Paths

| # | Condition | Region |
|---|-----------|--------|
| 1 | X > 0, Y > 0, \|X\| >= \|Y\| | Octant 1 |
| 2 | X > 0, Y > 0, \|X\| < \|Y\|  | Octant 2 |
| 3 | X < 0, Y > 0, \|X\| < \|Y\|  | Octant 3 |
| 4 | X < 0, Y > 0, \|X\| >= \|Y\| | Octant 4 |
| 5 | X < 0, Y < 0, \|X\| >= \|Y\| | Octant 5 |
| 6 | X < 0, Y < 0, \|X\| < \|Y\|  | Octant 6 |
| 7 | X > 0, Y < 0, \|X\| < \|Y\|  | Octant 7 |
| 8 | X > 0, Y < 0, \|X\| >= \|Y\| | Octant 8 |
| 9 | X == 0, Y != 0 | Forward / Backward |
| 10 | X != 0, Y == 0 | Left / Right |
| 11 | X == 0, Y == 0 | STOP |

## Known Issues

- STOP, Forward/Backward, and Left/Right use floating-point equality (`== 0`).
  These branches are only reachable if the caller explicitly zeroes the inputs
  via an offset workaround in `main.cpp`. Fragile under ADC noise.
- `absT<float>` hardcoded inside a `<real>` template — truncates silently if
  instantiated with `double`.
- Constructor does not initialize member variables.

## Delta Equation

```cpp
real delta = absT<float>(absT<float>(_inputY) - absT<float>(_inputX));
```

The double `absT` wrap is intentional. The outer wrap is necessary because
subtracting two non-negative values can produce a negative result when inputs
carry opposite signs across quadrants.

---

*Carpenter Software — carpentersoftware.com | MageMCU — github.com/MageMCU*
