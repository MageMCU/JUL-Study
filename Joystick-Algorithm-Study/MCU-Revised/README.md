# MCU-Revised — Joystick.h

**Platform:** Arduino Uno / Atmega328P / PlatformIO / C++  
**Author:** Jesse Carpenter (carpentersoftware.com) — MageMCU

---

## Purpose

`MCU-Revised/Joystick.h` is the Arduino/C++ structural equivalent of
`Unity-Revised/Algorithm.cs`.

It preserves the standard `Algorithm<real>` class interface used by this study file.
The production file (`Code - Joystick Uno L298N/include/Joystick.h`) uses the same
interface under the class name `Joystick<real>`.

- `UpdateInputs(real inputX, real inputY)`
- `OutLeft()`
- `OutRight()`
- `OutOctant()` (returns `int`, 0 to 8)
- `IsLeftForward()`
- `IsRightForward()`

and mirrors the Unity-Revised core logic:

- tolerance-based center stop
- `delta = abs(abs(x) - abs(y))`
- explicit X-dominant and Y-dominant branch hierarchy
- octant assignment (0 to 8)

## What `_octant` Is Used For

`_octant` stores the active joystick region as an integer state.

- **0** = center dead zone / stop
- **1-8** = active directional octant

`OutOctant()` exposes this state so downstream MCU code can use simple integer conditionals for:

- kinematic/control branching
- deterministic movement-state handling
- diagnostics and serial/debug reporting

---

## Relationship to Other Implementations

- `Unity-Revised/Algorithm.cs` -> source-equivalent branch structure (non-compact form)
- `Unity-Compact/JoystickAlgorithm.cs` -> optimized compact variant
- `MCU-Compact/Joystick.h` -> compact C++ port equivalent to Unity-Compact
- `MCU-Original/Joystick.h` -> historical original implementation

---

> **Status:** Added as the MCU counterpart to Unity-Revised for one-to-one code-structure comparison in this study repository.
