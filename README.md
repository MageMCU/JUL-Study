# JUL-Study

The first version of the Joystick-Uno-L298N code and the study of the joystick algorithm, as of July 13, 2026. The study compares the original algorithm with the revised and compact forms (MCU and Unity) and verifies them with a 20 point test. It is kept for readers of the earlier articles and of the simulation video, with the documents reduced to what is needed to use it.

The code that continues this work, with the revised algorithm, the experiments of Article 1004, and the setup of Article 1009, is maintained in [Joystick-Uno-L298N](https://github.com/MageMCU/Joystick-Uno-L298N).

- namespace **csjc** — **C**arpenter **S**oftware - **J**esse **C**arpenter

---

## Testing Platform

- MCU: **Atmega328P** (Arduino Uno)
- IDE/Build: **PlatformIO**
- Editor: **Visual Studio Code**

---

## Hardware

**MCU — Atmega328P (Arduino Uno)**
The Atmega328P chip can be pulled out and used separately on a breadboard. Purchase directly from the Arduino online store. Beware of counterfeit boards.

**Joystick**
A 2-axis analog thumb joystick with a push button. Two analog ADC pins on the Uno read X and Y independently. Both axes span the full 0–5 V range, producing a rectangular (not circular) input space after mapping.

**L298N Motor Driver**
Dual H-bridge module (ST L298N). Strong drive capability, low heat, strong anti-interference. Controls two DC motors independently for differential drive.

---

## Repository Layout

```
Joystick-Uno-L298N/
├── Code - Joystick Uno L298N/    ← Active firmware project
│   ├── include/                  ← All header files
│   │   ├── Joystick.h            ← ORIGINAL algorithm header (do not modify)
│   │   └── ...
│   ├── src/
│   │   ├── Step1_Joystick/       ← Joystick wiring validation
│   │   └── Step2_JUL/            ← Full joystick + L298N motor control
│   └── platformio.ini
├── Joystick-Algorithm-Study/     ← Imported study/reference material
│   ├── MCU-Original/             ← Study copy of original algorithm
│   ├── MCU-Revised/              ← Study: revised (non-compact) MCU algorithm
│   ├── MCU-Compact/              ← Study: compact MCU algorithm (hardware tested)
│   ├── Unity-Revised/            ← Study: first C# revision (Unity)
│   ├── Unity-Compact/            ← Study: compact C# revision (Unity)
│   └── MCU Algorithms for Articles/ ← Cleaned article-ready copies (debug removed)
├── JUL-Study/                    ← Hardware test project for the study algorithms
└── README.md                     ← This file
```

---

## Testing Study Algorithms Without Replacing `Joystick.h`

The original `Joystick.h` (class `Joystick`) stays untouched in `include/`. Study
algorithms can be tested alongside it using a preprocessor build flag — no file
replacement needed, no `main.cpp` restructuring required.

### Step 1 — Copy the study header into `include/` under a unique name

Copy the header you want to test from `Joystick-Algorithm-Study/` into
`Code - Joystick Uno L298N/include/` with a distinct filename:

| Study source | Copy to include/ as |
|---|---|
| `MCU-Revised/Joystick.h` | `Algorithm_Study_Revised.h` |
| `MCU-Compact/Joystick.h` | `Algorithm_Study_Compact.h` |

### Step 2 — Add a selector block to your `main.cpp`

At the top of `Code - Joystick Uno L298N/src/Step2_JUL/main.cpp`, replace the direct `#include "Joystick.h"` with:

```cpp
#if defined(ALGO_STUDY_REVISED)
    #include "Algorithm_Study_Revised.h"
    template <typename real> using ActiveAlgorithm = csjc::Algorithm<real>;
#elif defined(ALGO_STUDY_COMPACT)
    #include "Algorithm_Study_Compact.h"
    template <typename real> using ActiveAlgorithm = csjc::Algorithm<real>;
#else
    #include "Joystick.h"
    template <typename real> using ActiveAlgorithm = csjc::Joystick<real>;
#endif
```

Then declare and construct with `ActiveAlgorithm<float>` instead of `Joystick<float>`:

```cpp
ActiveAlgorithm<float> joystick;
// ...
joystick = ActiveAlgorithm<float>();
```

All method calls (`UpdateInputs`, `OutLeft`, `OutRight`, `IsLeftForward`,
`IsRightForward`) are identical across all three headers — no other code changes needed.

### Step 3 — Select the algorithm in `Common.h` (primary) or `platformio.ini` (alternative)

**Primary method — `Common.h`:**

Open `Code - Joystick Uno L298N/include/Common.h` and uncomment exactly one line
in the Algorithm Selection block:

```cpp
// Uncomment ONE line only:
#define ALGO_ORIGINAL           // Original Joystick.h (default)
// #define ALGO_STUDY_REVISED   // Study Revised (MCU-Revised)
// #define ALGO_STUDY_COMPACT   // Study Compact (MCU-Compact)
```

**Alternative method — `platformio.ini` build flags:**

Instead of editing `Common.h`, you can set a build flag in `platformio.ini`.
Comment out the default line and uncomment the desired study flag:

```ini
; --- Algorithm Selection ---
; Uncomment ONE line only:
build_flags = -std=gnu++11                              ; Original Joystick.h
; build_flags = -std=gnu++11 -DALGO_STUDY_REVISED      ; Study Revised (MCU-Revised)
; build_flags = -std=gnu++11 -DALGO_STUDY_COMPACT      ; Study Compact (MCU-Compact)
```

Both methods work independently. `Common.h` is the recommended approach for day-to-day
switching. The `platformio.ini` flag overrides `Common.h` if both are set.

---

## Algorithm Study — Quick Reference

The `Joystick-Algorithm-Study/` folder contains a complete research and verification
record. Key files:

- `Joystick-Algorithm-Study/README.md` — Full study report (root cause analysis, test results, speed benchmarks)
- `Joystick-Algorithm-Study/QA-Report.md` — 20-point input/output verification across all implementations

---

## Simulation and Articles

- [Joystick Algorithm Simulation (YouTube)](https://www.youtube.com/watch?v=maIHbdbDBwo&t=2s)
- Article Updates and Rewrites Coming...
- New Articles on the Way

---

## Status and Active Development

> **Hardware testing is complete.** The Arduino Uno with L298N motor driver
> has been tested across all joystick positions and motion types. The
> tolerance-based dead zone eliminates ADC noise without lag, and motor
> control is smooth and responsive throughout the full input range.
> Results are recorded in `Joystick-Algorithm-Study/MCU-Compact/README.md`
> and `Joystick-Algorithm-Study/QA-Report.md`.

This repository is no longer updated. New work, corrections, and releases are in
[Joystick-Uno-L298N](https://github.com/MageMCU/Joystick-Uno-L298N).

---



## Credits

Jesse Carpenter, with Claude (Anthropic), GitHub Copilot, and Google Gemini, June to July 2026.

Please follow the Disclaimer and Terms posted in this repository before
experimenting with live hardware. Safety comes first.
   
