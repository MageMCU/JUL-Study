# JUL-Study — Active Hardware Testing Project

namespace **csjc**

## `include/` — Header Files

| File | Purpose |
|---|---|
| `Bitwise.h` | Bit-manipulation utilities |
| `BusI2C.h` | I²C bus helpers |
| `Button.h` | Momentary push-button handler |
| `Common.h` | Shared constants, algorithm selection, and debug flags |
| `Headers.h` | Aggregated include for convenience |
| `Joystick.h` | **Original** joystick algorithm (class `Joystick`, namespace `csjc`) |
| `L298N.h` | L298N motor-driver interface |
| `LinearMap.h` | Linear range-mapping utility |
| `MiscMath.h` | General math helpers |
| `Switch.h` | Toggle/switch handler |
| `Timer.h` | Non-blocking millisecond timer |
| `TypeConv.h` | Type-conversion helpers |
| `Vector3.h` | 3-component vector math |

> **Study algorithm headers** — All three study headers live in named subfolders
> inside `include/` and are included directly by path in `main.cpp`. No file
> renaming or manual copying is required.

| Subfolder | File | Algorithm |
|---|---|---|
| `MCU-Original/` | `Joystick.h` | Original 11-path (class `Algorithm`) |
| `MCU-Revised/` | `Joystick.h` | Revised 9-path, non-compact (class `Algorithm`) |
| `MCU-Compact/` | `Joystick.h` | Compact 9-path, hardware tested (class `Algorithm`) |

## `src/` — Firmware Programs

| Folder | File | Status |
|---|---|---|
| `DEP/` | `BusI2C.cpp` | I²C dependency (compiled with all targets) |
| `Step1_Joystick/` | `main.cpp` | Joystick wiring validation — **tested** |
| `Step2_JUL/` | `main.cpp` | Full Joystick + L298N motor control — **tested** |

## Root

| File | Purpose |
|---|---|
| `platformio.ini` | Build configuration (Windows — `upload_port = COM4`) |

## Notes

- Active hardware testing workspace — Windows, upload port `COM4`.
- `DEBUG_JOYSTICK` is active by default in `Common.h` — sets 3000ms loop and enables serial output. Comment it out to restore 100ms motor control loop speed.
- Study algorithm headers are in subfolders (`MCU-Original/`, `MCU-Revised/`, `MCU-Compact/`) inside `include/` — selected directly by path in `main.cpp`.

---

## Key Differences vs `Code - Joystick Uno L298N`

| Item | `JUL-Study` | `Code - Joystick Uno L298N` |
|------|------------|----------------------------|
| **Upload port** | `COM4` (Windows) | `/dev/ttyACM0` (Linux) |
| **Study header location** | Subfolders inside `include/` (`MCU-Original/`, `MCU-Revised/`, `MCU-Compact/`) | Flat `include/` — expects renamed copies (`Algorithm_Study_Revised.h` etc.) |
| **Selector — ALGO_ORIGINAL** | Explicit `#if defined(ALGO_ORIGINAL)` → `MCU-Original/Joystick.h` | Falls to `#else` → `Joystick.h` (implicit) |
| **Selector — ALGO_STUDY_REVISED** | `#include "MCU-Revised/Joystick.h"` (subfolder path) | `#include "Algorithm_Study_Revised.h"` (flat, renamed) |
| **DEBUG_JOYSTICK** | **Active** — serial output on, 3000ms loop | Commented out — 100ms loop, no serial output |
| **Algorithm selection flags in platformio.ini** | Not present | Commented-out `-DALGO_STUDY_REVISED` / `-DALGO_STUDY_COMPACT` lines present |
| **Step3_JULE comment** | `(Step3_JULE removed - no longer used in this repository.)` | `(Step3_JULE removed)` |
