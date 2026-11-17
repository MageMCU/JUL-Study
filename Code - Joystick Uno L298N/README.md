# Joystick Uno L298N (JUL) — Code Folder

namespace **csjc**

## `include/` — Header Files

| File | Purpose |
|---|---|
| `Bitwise.h` | Bit-manipulation utilities |
| `BusI2C.h` | I²C bus helpers |
| `Button.h` | Momentary push-button handler |
| `Common.h` | Shared constants and type definitions |
| `Headers.h` | Aggregated include for convenience |
| `Joystick.h` | **Original** joystick algorithm (class `Joystick`, namespace `csjc`) |
| `L298N.h` | L298N motor-driver interface |
| `LinearMap.h` | Linear range-mapping utility |
| `MiscMath.h` | General math helpers |
| `Switch.h` | Toggle/switch handler |
| `Timer.h` | Non-blocking millisecond timer |
| `TypeConv.h` | Type-conversion helpers |
| `Vector3.h` | 3-component vector math |

> **Study algorithm headers** — If you are testing a study algorithm, also drop
> `Algorithm_Study_Revised.h` or `Algorithm_Study_Compact.h` here. See the root
> `README.md` for step-by-step instructions.

## `src/` — Firmware Programs

| Folder | File | Status |
|---|---|---|
| `DEP/` | `BusI2C.cpp` | I²C dependency (compiled with all targets) |
| `Step1_Joystick/` | `main.cpp` | Joystick wiring validation — **tested** |
| `Step2_JUL/` | `main.cpp` | Full Joystick + L298N motor control — **tested** |

## Root

| File | Purpose |
|---|---|
| `platformio.ini` | Build configuration (environment, flags, upload settings) |

## Notes

- Code updated 20240820 (Article 1009).

---

## Key Differences vs `JUL-Study`

| Item | `Code - Joystick Uno L298N` | `JUL-Study` |
|------|----------------------------|------------|
| **Upload port** | `/dev/ttyACM0` (Linux) | `COM4` (Windows) |
| **Study header location** | Flat `include/` — expects renamed copies (`Algorithm_Study_Revised.h` etc.) | Subfolders inside `include/` (`MCU-Original/`, `MCU-Revised/`, `MCU-Compact/`) |
| **Selector — ALGO_ORIGINAL** | Falls to `#else` → `Joystick.h` (implicit) | Explicit `#if defined(ALGO_ORIGINAL)` → `MCU-Original/Joystick.h` |
| **Selector — ALGO_STUDY_REVISED** | `#include "Algorithm_Study_Revised.h"` (flat, renamed) | `#include "MCU-Revised/Joystick.h"` (subfolder path) |
| **DEBUG_JOYSTICK** | Commented out — 100ms loop, no serial output | **Active** — serial output on, 3000ms loop |
| **Algorithm selection flags in platformio.ini** | Commented-out `-DALGO_STUDY_REVISED` / `-DALGO_STUDY_COMPACT` lines present | Not present |
| **Step3_JULE comment** | `(Step3_JULE removed)` | `(Step3_JULE removed - no longer used in this repository.)` |
