# MCU-Revised — Joystick.h

**Platform:** Arduino Uno / Atmega328P / PlatformIO / C++  
**Author:** Jesse Carpenter (carpentersoftware.com) — MageMCU

---

## Purpose

`MCU-Revised/Joystick.h` is the Arduino/C++ structural equivalent of
`Unity-Revised/Algorithm.cs`.

It preserves the standard `Algorithm<real>` class interface used by the MCU code:

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

`_octant` is an integer state that identifies which joystick region is active.

- **0** = center stop
- **1-8** = directional octants

`OutOctant()` exists so callers can branch cleanly on integer octant state for:

- control/kinematics decisions
- movement-state transitions
- debug and test assertions

---

## Relationship to Other Implementations

- `Unity-Revised/Algorithm.cs` -> source-equivalent branch structure (non-compact form)
- `Unity-Compact/JoystickAlgorithm.cs` -> optimized compact variant
- `MCU-Compact/Joystick.h` -> compact C++ port equivalent to Unity-Compact
- `MCU-Original/Joystick.h` -> historical original implementation

---

> **Status:** Added as the MCU counterpart to Unity-Revised for one-to-one code-structure comparison in this study repository.
