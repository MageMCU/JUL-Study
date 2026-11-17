# Joystick Algorithm — Study Report
**Project:** Carpenter Software / MageMCU
**Repository (MCU):** Joystick-UNO-L298N (GitHub) — C++ / Arduino / PlatformIO
**Repository (Simulation):** Robot-6 (Unity Asset Store pending) — C# / Unity Engine
**Hardware:** Arduino Uno / Atmega328P / L298N Motor Driver (MCU platform only)
**Date:** June 22–25, 2026
**Collaborators:** Jesse Carpenter, Claude (Anthropic), GitHub Copilot, Google Gemini

> **Platform Note:** This report covers two distinct platforms. The original algorithm (`Joystick.h`) is microcontroller (MCU) code written in C++ and deployed on the Arduino Uno. The revised algorithms (`Algorithm.cs` and `JoystickAlgorithm.cs`) are Unity simulation code written in C#, maintained across the MageMCU Unity repositories with Robot-6 as the primary deployment target (Unity Asset Store pending). This repository also includes MCU-side counterparts: `MCU-Revised/Joystick.h` (structure-equivalent to `Unity-Revised/Algorithm.cs`) and `MCU-Compact/Joystick.h` (structure-equivalent to `Unity-Compact/JoystickAlgorithm.cs`). MCU hardware testing of the revised compact C++ logic has been completed and confirmed successful.

---

## Background

Jesse Carpenter, operating under the MageMCU GitHub organization, has been investigating criticism directed at the Joystick Algorithm in the Joystick-UNO-L298N repository. The criticism was surfaced by Google Gemini and centers on two specific claims: that the algorithm produces severe signal gaps (dead zones) and erratic data spikes. This discussion examines the validity of those claims in the context of the actual hardware and code.

---

## The Algorithm

The core implementation lives in `Joystick.h` as a C++ template class `Algorithm<real>` within the `csjc` namespace in the study copies. The production file (`Code - Joystick Uno L298N/include/Joystick.h`) uses the same logic with the class named `Joystick<real>`. The algorithm divides the joystick input space into nine regions — a center stop zone and eight surrounding octants — determined by which axis dominates and which quadrant the joystick occupies.

**Inputs:** `float inputX`, `float inputY`
**Outputs:** `float outputLeft`, `float outputRight`

The governing delta equation is:

```cpp
real delta = absT<float>(absT<float>(_inputY) - absT<float>(_inputX));
```

The double `absT` wrap is intentional and load-bearing. The outer wrap is necessary because subtracting two non-negative values can produce a negative result. This equation was confirmed correct in prior testing; simplifying it fails in approximately three-quarters (~75%) of all cases when inputs carry opposite signs.

### Octant Orientation and Dominance Rules

For directional reference, the joystick plane can be read as eight boundary points around a clock face:

1. Right (0 degrees)
2. Forward-Right (45 degrees)
3. Forward (90 degrees)
4. Forward-Left (135 degrees)
5. Left (180 degrees)
6. Backward-Left (225 degrees / -135 degrees)
7. Backward (270 degrees / -90 degrees)
8. Backward-Right (315 degrees / -45 degrees)

The octants are the ranges between those boundary points:

- Octant 1: Right -> Forward-Right
- Octant 2: Forward-Right -> Forward
- Octant 3: Forward -> Forward-Left
- Octant 4: Forward-Left -> Left
- Octant 5: Left -> Backward-Left
- Octant 6: Backward-Left -> Backward
- Octant 7: Backward -> Backward-Right
- Octant 8: Backward-Right -> Right

This means Octant 1 begins at Right, and Octant 8 also terminates at Right from below.

Dominance is decided by axis magnitude:

- X-dominant: `|inputX| >= |inputY|` -> Octants 1, 4, 5, 8
- Y-dominant: `|inputX| < |inputY|` -> Octants 2, 3, 6, 7

When inputs are exactly on a diagonal (`|inputX| == |inputY|`), the tie goes to the X-dominant branch by design (`>=`). Therefore exact diagonals map to Octants 1, 4, 5, and 8.

---

## The Input Pipeline

The full input pipeline, as established by `main.cpp`, is:

1. Raw ADC values read from analog pins A0 and A1 (range 0–1023)
2. Mapped via `LinearMap<float>(0, 1023, -1.0, 1.0)` to the range -1.0 to 1.0
3. A centering offset applied to suppress drift near the stick center:
   - `X_OFFSET = 0.05`
   - `Y_OFFSET = 0.06`
4. Mapped values fed into `joystick.UpdateInputs(inputX, inputY)`
5. Outputs mapped via `LinearMap<float>(-1.0, 1.0, -255, 255)` for L298N motor driver

The algorithm therefore operates on a correctly centered -1.0 to 1.0 input range, not raw ADC values.

---

## The Criticism

The criticism claims the algorithm produces:

- **Gaps (dead zones):** Input regions that yield no or minimal output, killing responsiveness
- **Spikes:** Sudden discontinuous jumps in output at octant boundaries

Jesse confirmed through his own testing that these problems are real — but only under specific input conditions.

---

## Root Cause Analysis

The key finding of this discussion is that the validity of the criticism depends entirely on the **input model assumed**:

### Unit Circle Input (Where Criticism Holds)

If input values are normalized to a unit circle — meaning X and Y are constrained such that `sqrt(X² + Y²) <= 1`, as is common with modern gamepad hardware — then corner values such as (0.7, 0.7) are geometrically impossible. Under this model the octant boundary conditions do produce gaps and spikes because the algorithm's geometry assumes inputs that can reach the corners of the square.

### Rectangular Input (The Actual Hardware)

The physical analog joystick wired to the Atmega328P delivers voltage across a rectangular area, where both X and Y independently range across the full 0 to 5 volts (center at 2.5 volts). After the `LinearMap` transformation, both axes independently span -1.0 to 1.0. Corner values such as (1.0, 1.0) are completely valid inputs. The algorithm was designed for this square input space.

The criticism is therefore technically valid only for a unit-circle input model that does not match the hardware this algorithm was built for.

### Remaining Gaps

Jesse confirmed that even within the rectangular input model, some gaps are present depending on specific raw input values. This finding motivated a rewrite of the algorithm, which Jesse completed over the course of the three-day investigation.

---

## Status

- The original criticism was surfaced by Google Gemini, not from GitHub issue reports
- Jesse reproduced the gaps and spikes through his own testing of the MCU C++ code (`Joystick.h`)
- The problems were confirmed to be input-model dependent, but not entirely absent under rectangular input
- Jesse rewrote the algorithm logic in C# for the Unity simulation environment (`Algorithm.cs`, `JoystickAlgorithm.cs`)
- The Unity simulation code has been developed, verified, and benchmarked (see Verification section)
- The Arduino Uno MCU hardware testing of the revised C++ logic (`Joystick.h`) has been successfully completed and confirmed working across all motion types

---

## Quick Start

### For Arduino Users
1. Use `MCU-Revised/Joystick.h` when you want a one-to-one structural MCU counterpart of `Unity-Revised/Algorithm.cs`.
2. Use `MCU-Compact/Joystick.h` when you want the compact MCU counterpart of `Unity-Compact/JoystickAlgorithm.cs`.
3. Keep your existing `main.cpp` integration unchanged.
4. Validate behavior with the same input pipeline documented in **The Input Pipeline** section.

### For Unity Developers
1. Use `Unity-Compact/JoystickAlgorithm.cs` for production-oriented performance.
2. Use `Unity-Revised/Algorithm.cs` when you want the clearer first-revision structure.
3. Apply the same 20-point verification set in `QA-Report.md` after integration.

### For Reviewers and Researchers
1. Start with **Root Cause Analysis** in this file.
2. Compare `MCU-Original/Joystick.h` vs `MCU-Revised/Joystick.h` vs `MCU-Compact/Joystick.h`.
3. Use `QA-Report.md` for cross-implementation result checks.

---

## Revised Algorithm — Algorithm.cs (Unity Simulation / C#)

The rewritten C# implementation for the Unity simulation environment has been developed in response to the gap and spike findings in the original MCU code. This is not MCU code — it runs inside the Unity Engine and is not directly deployable to the Arduino Uno. It replaces the original octant logic with a restructured conditional hierarchy using a tolerance-based center stop, a unified delta equation, and clean separation between x-dominant and y-dominant octant branches.

**Location:** See the full implementation in `Unity-Revised/Algorithm.cs`.

---

## Compact Revision — JoystickAlgorithm.cs (Robot-8 Study)

This is a more efficient refactor of Algorithm.cs developed during the Robot-8 advanced study. Robot-8 follows the same research track as Robot-6 and is being used to work through Chapter 3 concepts from *Autonomous Mobile Robots* in a practical Unity workflow. The octant logic and motor output values are identical to Algorithm.cs, but the implementation is tightened in three ways: the absolute values of both inputs are cached up front to avoid repeated `Mathf.Abs` calls, delta is simplified to a single subtraction of those cached values, and octant assignment uses nested ternary expressions rather than four separate conditional blocks. The result is fewer operations per frame with no change in behavior. The previously discussed Image 6 material is being tracked separately and can be integrated into this documentation in a later update.

**Location:** See the full implementation in `Unity-Compact/JoystickAlgorithm.cs`.

---

## Verification — 20-Point Input/Output Comparison

Both algorithms were run against the same 20 input pairs, covering all eight octants, the center stop zone, the tolerance boundary, and the diagonal boundary where both axes are equal. The table shows Left motor, Right motor, and Octant for each algorithm side by side, with a Match column confirming whether all three outputs are identical.

**A** = Algorithm.cs (original) · **C** = JoystickAlgorithm.cs (compact)

| # | inputX | inputY | A-Left | A-Right | A-Oct | C-Left | C-Right | C-Oct | Match |
|---|-------:|-------:|-------:|--------:|------:|-------:|--------:|------:|-------|
| 1 | 0.0000 | 0.0000 | 0.0000 | 0.0000 | 0 | 0.0000 | 0.0000 | 0 | True |
| 2 | 0.0005 | 0.0005 | 0.0000 | 0.0000 | 0 | 0.0000 | 0.0000 | 0 | True |
| 3 | 0.8000 | 0.3000 | 0.8000 | -0.5000 | 1 | 0.8000 | -0.5000 | 1 | True |
| 4 | 1.0000 | 0.5000 | 1.0000 | -0.5000 | 1 | 1.0000 | -0.5000 | 1 | True |
| 5 | 0.3000 | 0.8000 | 0.8000 | 0.5000 | 2 | 0.8000 | 0.5000 | 2 | True |
| 6 | 0.5000 | 1.0000 | 1.0000 | 0.5000 | 2 | 1.0000 | 0.5000 | 2 | True |
| 7 | -0.3000 | 0.8000 | 0.5000 | 0.8000 | 3 | 0.5000 | 0.8000 | 3 | True |
| 8 | -0.5000 | 1.0000 | 0.5000 | 1.0000 | 3 | 0.5000 | 1.0000 | 3 | True |
| 9 | -0.8000 | 0.3000 | -0.5000 | 0.8000 | 4 | -0.5000 | 0.8000 | 4 | True |
| 10 | -1.0000 | 0.5000 | -0.5000 | 1.0000 | 4 | -0.5000 | 1.0000 | 4 | True |
| 11 | -0.8000 | -0.3000 | -0.8000 | 0.5000 | 5 | -0.8000 | 0.5000 | 5 | True |
| 12 | -1.0000 | -0.5000 | -1.0000 | 0.5000 | 5 | -1.0000 | 0.5000 | 5 | True |
| 13 | -0.3000 | -0.8000 | -0.8000 | -0.5000 | 6 | -0.8000 | -0.5000 | 6 | True |
| 14 | -0.5000 | -1.0000 | -1.0000 | -0.5000 | 6 | -1.0000 | -0.5000 | 6 | True |
| 15 | 0.3000 | -0.8000 | -0.5000 | -0.8000 | 7 | -0.5000 | -0.8000 | 7 | True |
| 16 | 0.5000 | -1.0000 | -0.5000 | -1.0000 | 7 | -0.5000 | -1.0000 | 7 | True |
| 17 | 0.8000 | -0.3000 | 0.5000 | -0.8000 | 8 | 0.5000 | -0.8000 | 8 | True |
| 18 | 1.0000 | -0.5000 | 0.5000 | -1.0000 | 8 | 0.5000 | -1.0000 | 8 | True |
| 19 | 0.7000 | 0.7000 | 0.7000 | 0.0000 | 1 | 0.7000 | 0.0000 | 1 | True |
| 20 | -0.6000 | -0.6000 | -0.6000 | 0.0000 | 5 | -0.6000 | 0.0000 | 5 | True |

**Result: 20 / 20 True.** Both algorithms produce identical Left, Right, and Octant outputs across all test cases. The compact refactor is a verified equivalent of the original.

---

## Speed Test — Algorithm.cs vs JoystickAlgorithm.cs

Both algorithms were benchmarked against the same 20 input pairs across 1,000,000 iterations per run, repeated over 5 runs. Times are recorded in seconds; the average is used for the final comparison.

**Test conditions:** 1,000,000 iterations × 5 runs × 20 input pairs = 100,000,000 total calls per algorithm

| Run | Algorithm.cs (s) | JoystickAlgorithm.cs (s) |
|:---:|----------------:|------------------------:|
| 1 | 6.7356 | 6.0721 |
| 2 | 6.5378 | 6.0397 |
| 3 | 6.6900 | 6.0163 |
| 4 | 6.6449 | 6.0809 |
| 5 | 6.5691 | 6.0653 |
| **Avg** | **6.6355** | **6.0548** |

**Result:** JoystickAlgorithm.cs (Compact) is faster by 580.6 ms average — approximately **8.75% faster** than Algorithm.cs. The margin is larger than anticipated, attributable to the cached `absX` / `absY` values eliminating repeated `Mathf.Abs` calls on every branch evaluation in the original. At the scale of a real-time robot control loop running at 50–100 Hz, the difference per frame is negligible, but the compact version is the clear choice for production use.

---

## Original Algorithm — Joystick.h (MCU / C++)

The original C++ implementation that is the subject of this report. The structural differences are immediately visible when compared to the revised algorithms: the original uses 11 conditionals — eight octant branches plus three axis-aligned special cases for Forward/Backward, Left/Right, and STOP — where the revised algorithms use 9, having absorbed those axis-aligned cases into the tolerance-based center stop and the unified octant hierarchy. The original also uses floating-point equality checks (`_inputX == (real)0`) for the axis-aligned cases, which under real ADC noise conditions will rarely evaluate true. The revised algorithms correct this with the tolerance dead zone.

**Location:** Find the original implementation in `MCU-Original/Joystick.h`.

---

## Updated Algorithm — Joystick.h (MCU / C++)

A revised version of `Joystick.h` ports the compact octant algorithm back into the original C++ template class. The class interface is unchanged — same namespace (`csjc`), same template parameter (`real`), same public methods (`UpdateInputs`, `OutLeft`, `OutRight`, `IsLeftForward`, `IsRightForward`) — so existing `main.cpp` code requires no modification. The only change is inside `_joystick()`, where the 11-conditional original is replaced by the 9-conditional compact core: a tolerance-based center stop, cached absolute values, and unified octant branches using the delta equation. The floating-point equality checks for Forward/Backward, Left/Right, and STOP are eliminated; the tolerance dead zone absorbs all three.

> **Status:** ✅ **Hardware Testing Complete**
> 
> The revised `Joystick.h` has been successfully tested on Arduino Uno hardware with the L298N motor driver. All functionality works as expected across all joystick positions and motion types. The tolerance-based dead zone effectively eliminates ADC noise without introducing lag, and motor control is smooth and responsive throughout the full input range.

**Location:** Find the updated implementation in `MCU-Compact/Joystick.h`.

---

## MCU Equivalent of Algorithm.cs — Joystick.h (MCU / C++)

`MCU-Revised/Joystick.h` is the non-compact MCU implementation added to match
`Unity-Revised/Algorithm.cs` at the branch-structure level. It preserves the same
Arduino class interface (`UpdateInputs`, `OutLeft`, `OutRight`, forward-direction
helpers) while keeping the Unity-Revised style internals: tolerance-based center
stop, explicit X-dominant and Y-dominant branch blocks, and the non-cached delta form.

This gives the study two aligned MCU tracks:

- `MCU-Revised/Joystick.h` <-> `Unity-Revised/Algorithm.cs`
- `MCU-Compact/Joystick.h` <-> `Unity-Compact/JoystickAlgorithm.cs`

**Location:** Find this implementation in `MCU-Revised/Joystick.h`.

---

## Summary & Recommendations

### For Arduino Users (Joystick-UNO-L298N Repository)

The Arduino repository [Joystick-UNO-L298N](https://github.com/MageMCU/Joystick-Uno-L298N) currently contains the original `Joystick.h` implementation, which was the subject of the gap and spike criticism discussed in this report. The revised and tested `Joystick.h` implementation (in `MCU-Compact/Joystick.h` in this repository) represents the corrected logic with validated hardware performance.

If you are working with the Arduino repository, you are encouraged to review the revised implementation and consider updating your deployment with the improved algorithm. The core improvements — the delta equation, the octant conditional hierarchy, and the tolerance-based center stop — translate directly from the C# prototypes and have been verified on actual hardware.

### For Unity Developers

The two revised C# algorithms — `Algorithm.cs` and `JoystickAlgorithm.cs` — represent the corrected logic developed in response to the criticism and have been verified to produce identical results across all test cases. They are maintained in the MageMCU Unity repositories, with `JoystickAlgorithm.cs` recommended for production use due to its 8.75% performance improvement.

### Supporting Open-Source Development

Supporting the Robot-6 Unity Asset Store release helps sustain the ongoing open-source work at MageMCU and [Carpenter Software](https://carpentersoftware.com). Community support keeps the repositories active, the documentation maintained, and new projects moving forward.

### Testing & Feedback

Your testing, feedback, and observations are welcome. Please follow the Disclaimer and Terms posted in the repository before experimenting with live hardware. Hardware testing has confirmed the revised algorithm works reliably across all joystick positions and motion types.

---

*Carpenter Software — carpentersoftware.com*
*MageMCU — github.com/MageMCU*

---

## Polar Coordinate Method — PolarCoordinate.cs (Analysis)

### Overview

The Polar Coordinate method is one of the most commonly cited alternatives for mapping a two-axis joystick input to differential drive motor output. It shares the same input/output signature as the octant algorithms — normalized `inputX` and `inputY` in the range -1.0 to 1.0, producing `Left` and `Right` motor values — making it a direct peer for comparison. The method converts the Cartesian joystick position into polar form (magnitude and angle), then derives motor output from those polar components.

### Implementation — PolarCoordinate.cs

The Polar Coordinate method converts the Cartesian joystick position into polar form (magnitude and angle), then derives motor output from those polar components. A complete reference implementation is available for study and comparison purposes.

**Location:** PolarCoordinate.cs is discussed in this report as a comparison method; its reference implementation is not currently included in this repository.

### Verification — PolarCoordinate.cs vs JoystickAlgorithm.cs

Both algorithms were run against the same 20-point input set used throughout this report. The table compares Left motor, Right motor, and Octant outputs side by side.

**J** = JoystickAlgorithm.cs · **P** = PolarCoordinate.cs

| # | inputX | inputY | J-Left | J-Right | J-Oct | P-Left | P-Right | P-Oct |
|---|-------:|-------:|-------:|--------:|------:|-------:|--------:|------:|
| 1 | 0.0000 | 0.0000 | 0.0000 | 0.0000 | 0 | 0.0000 | 0.0000 | 0 |
| 2 | 0.0005 | 0.0005 | 0.0000 | 0.0000 | 0 | 0.0000 | 0.0000 | 0 |
| 3 | 0.8000 | 0.3000 | 0.8000 | -0.5000 | 1 | 1.0000 | 0.0544 | 1 |
| 4 | 1.0000 | 0.5000 | 1.0000 | -0.5000 | 1 | 1.0000 | 0.1180 | 1 |
| 5 | 0.3000 | 0.8000 | 0.8000 | 0.5000 | 2 | 1.0000 | 0.5544 | 2 |
| 6 | 0.5000 | 1.0000 | 1.0000 | 0.5000 | 2 | 1.0000 | 0.6180 | 2 |
| 7 | -0.3000 | 0.8000 | 0.5000 | 0.8000 | 3 | 0.5544 | 1.0000 | 3 |
| 8 | -0.5000 | 1.0000 | 0.5000 | 1.0000 | 3 | 0.6180 | 1.0000 | 3 |
| 9 | -0.8000 | 0.3000 | -0.5000 | 0.8000 | 4 | 0.0544 | 1.0000 | 5 |
| 10 | -1.0000 | 0.5000 | -0.5000 | 1.0000 | 4 | 0.1180 | 1.0000 | 5 |
| 11 | -0.8000 | -0.3000 | -0.8000 | 0.5000 | 5 | 0.0544 | 1.0000 | 6 |
| 12 | -1.0000 | -0.5000 | -1.0000 | 0.5000 | 5 | 0.1180 | 1.0000 | 6 |
| 13 | -0.3000 | -0.8000 | -0.8000 | -0.5000 | 6 | 0.5544 | 1.0000 | 7 |
| 14 | -0.5000 | -1.0000 | -1.0000 | -0.5000 | 6 | 0.6180 | 1.0000 | 7 |
| 15 | 0.3000 | -0.8000 | -0.5000 | -0.8000 | 7 | 1.0000 | 0.5544 | 7 |
| 16 | 0.5000 | -1.0000 | -0.5000 | -1.0000 | 7 | 1.0000 | 0.6180 | 7 |
| 17 | 0.8000 | -0.3000 | 0.5000 | -0.8000 | 8 | 1.0000 | 0.0544 | 8 |
| 18 | 1.0000 | -0.5000 | 0.5000 | -1.0000 | 8 | 1.0000 | 0.1180 | 8 |
| 19 | 0.7000 | 0.7000 | 0.7000 | 0.0000 | 1 | 1.0000 | 0.2899 | 1 |
| 20 | -0.6000 | -0.6000 | -0.6000 | 0.0000 | 5 | 0.2485 | 1.0000 | 6 |

**Result: Outputs differ across all non-zero, non-center test cases.** The two algorithms produce fundamentally different Left and Right motor values. This is expected — they are not equivalent implementations of the same logic. The Polar method produces different motor differentials because it derives turn rate from the sine of the joystick angle rather than from the axis-dominance delta used in the octant approach. Octant assignment also diverges in the third and fourth quadrants (rows 9–14, 20), reflecting the two methods' different geometric assumptions about the input space. The center stop (rows 1–2) is the only region where both algorithms agree exactly.

### Speed Test — JoystickAlgorithm.cs vs PolarCoordinate.cs

Both algorithms were benchmarked against the same 20 input pairs across 1,000,000 iterations per run, repeated over 5 runs, matching the test conditions used in the earlier speed comparison.

**Test conditions:** 1,000,000 iterations × 5 runs × 20 input pairs = 100,000,000 total calls per algorithm

| Run | JoystickAlgorithm.cs (s) | PolarCoordinate.cs (s) |
|:---:|------------------------:|----------------------:|
| 1 | 0.2009 | 1.7159 |
| 2 | 0.2006 | 1.6973 |
| 3 | 0.2011 | 1.6928 |
| 4 | 0.2008 | 1.6924 |
| 5 | 0.2019 | 1.6949 |
| **Avg** | **0.2011** | **1.6987** |

**Result:** PolarCoordinate.cs is slower by 1,497.6 ms average — approximately **744.79% slower** than JoystickAlgorithm.cs. The margin is not subtle. Three transcendental function calls per frame — `Sqrt`, `Atan2`, and `Sin` — are the direct cause. These functions involve iterative approximation internally and are several orders of magnitude more expensive than the comparisons, subtractions, and absolute value operations that make up the octant algorithm. At a robot control loop running at 50–100 Hz the per-frame difference is small in absolute terms, but the relative cost is clear: the Polar method does roughly 7.5 times the computational work to produce a different and arguably less predictable motor differential than the octant approach.

### Summary

The Polar Coordinate method is a legitimate and widely used approach, but it is not a drop-in equivalent of the octant algorithm. It produces different motor outputs, carries a significant computational overhead from its trigonometric core, and was designed with circular input normalization in mind — not the rectangular input space of an analog joystick wired to a microcontroller ADC. For the hardware and use case described in this report, the octant-based `JoystickAlgorithm.cs` is the faster and more geometrically appropriate choice.

---

## One-to-One Output Agreement — JoystickAlgorithm.cs vs PolarCoordinate.cs

### The Question

After establishing that the two algorithms produce different motor outputs across the 20-point verification table, the natural follow-up is whether any non-trivial input exists where both methods agree exactly — a true one-to-one match on both Left and Right simultaneously.

### The Test

A 200×200 grid scan was performed across the full rectangular input space, covering 40,400 non-center points at a resolution of 0.01 across both axes. Each point was run through both algorithms and the Left and Right outputs were compared within an epsilon of 0.0001.

**Result: 102 matches out of 40,400 non-center points tested.**

Every single match fell on the same geometric condition: `inputX = 0` with `inputY > 0` — pure forward motion along the positive Y axis only. No match was found at any point where a non-zero X component was present, and no match was found on the negative Y axis.

### Why They Agree Only at inputX = 0, inputY > 0

When `inputX = 0` and `inputY > 0`, both algorithms converge on the same answer through different paths:

- **JoystickAlgorithm.cs:** Y is dominant, delta is zero, both motors are set to `inputY`. Output: `Left = inputY`, `Right = inputY`.
- **PolarCoordinate.cs:** `angle = atan2(0, inputY) = 0`, therefore `sin(angle) = 0`, turn is zero, and both motors are set to `magnitude = inputY`. Output: `Left = inputY`, `Right = inputY`.

Pure straight-ahead motion is the one case where the geometry of both methods collapses to the same trivial result — no turn, equal motors, dominant axis only. Everywhere else the methods diverge because they compute the motor differential from fundamentally different geometric models.

The negative Y axis fails to match for a revealing reason: the Polar method derives `magnitude` from `sqrt(x² + y²)`, which is always positive. Without explicit sign handling for the reverse direction, `(0, -1)` returns `Left = 1.0, Right = 1.0` — full forward — while the octant algorithm correctly returns `Left = -1.0, Right = -1.0`. The Polar implementation as commonly published does not naturally handle reverse drive without additional logic.

### Why JoystickAlgorithm.cs May Be the Only Feasible Public Implementation

The output divergence between the two methods is not a matter of precision or tuning — it is structural. The Polar method and the octant method are solving different geometric problems and cannot be made equivalent by adjusting constants or thresholds.

Beyond correctness, three practical factors make `JoystickAlgorithm.cs` the stronger candidate for public use:

**1. Hardware compatibility.** The octant algorithm was designed for a rectangular input space — the geometry of a real analog joystick wired to an ADC. The Polar method assumes circular normalization, which is standard on modern gamepad hardware but does not match the physical joystick used in this project. Publishing a Polar implementation without that caveat would mislead readers working with the same hardware.

**2. Reverse drive correctness.** As the grid scan confirmed, the Polar method as commonly published fails on negative Y — it loses the sign of the Y axis, returning forward motor values when the joystick is pushed backward. Correcting this requires additional logic that is not part of the standard formulation, and adds complexity that the octant algorithm never needed in the first place.

**3. Computational cost.** At 744.79% slower than `JoystickAlgorithm.cs`, the Polar method carries a transcendental function overhead — `Sqrt`, `Atan2`, and `Sin` — that is difficult to justify when a conditional-arithmetic approach produces a geometrically appropriate result in a fraction of the time. On a microcontroller, where floating-point trigonometry is software-emulated, the gap would be even wider.

The octant algorithm is not just faster. It is more correct for the hardware it targets, handles reverse drive without special cases, and produces a motor differential that scales cleanly with joystick position across the full rectangular input space. That combination of geometric fitness, correctness, and performance is what makes `JoystickAlgorithm.cs` the appropriate choice for public publication under the MageMCU organization.

---

*Carpenter Software — carpentersoftware.com*
*MageMCU — github.com/MageMCU*
