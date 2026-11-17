# Unity-Revised — Algorithm.cs

**Platform:** Unity Engine / C#
**Environment:** Robot-6 (Unity Asset Store pending)
**Author:** Jesse Carpenter (carpentersoftware.com) — MageMCU

---

## Purpose

`Algorithm.cs` is the **first revised C# implementation** of the joystick-to-motor mapping algorithm, developed in response to the gap and spike findings in the original MCU C++ code (`MCU-Original/Joystick.h`). It runs inside the Unity Engine and is not directly deployable to the Arduino Uno. It replaces the original octant logic with a restructured conditional hierarchy using a tolerance-based center stop, a unified delta equation, and clean separation between x-dominant and y-dominant octant branches.

---

## How It Works

The algorithm divides the joystick input space into nine regions using 9 conditionals:

- **Octant 0 (center stop):** Both axes within tolerance — all motors stopped
- **X-dominant octants (1, 4, 5, 8):** `|inputX| >= |inputY|` — turning harder than driving straight
- **Y-dominant octants (2, 3, 6, 7):** `|inputX| < |inputY|` — driving more straight than turning

The delta equation:

```csharp
float delta = Mathf.Abs(Mathf.Abs(_inputX) - Mathf.Abs(_inputY));
```

The double `Mathf.Abs` is intentional and load-bearing — same reasoning as the original C++ double `absT`. It is preserved here exactly for correctness. The compact refactor (`Unity-Compact/JoystickAlgorithm.cs`) simplifies this by caching `absX` and `absY` first.

**Inputs:** `float inputX`, `float inputY` (normalized, range -1.0 to 1.0)
**Outputs:** `OutputLeftValue`, `OutputRightValue` (range -1.0 to 1.0), `OutputOctant` (0–8)

### Key Corrections Over MCU-Original

- Tolerance-based center stop replaces floating-point equality checks
- No floating-point equality comparisons anywhere in the logic
- 9 conditionals instead of 11 — Forward/Backward and Left/Right axis-aligned cases absorbed into the octant hierarchy
- Each octant branch is clearly labeled and commented

---

## Public API

```csharp
void Initialize()               // Call before first Update()
void Update(float x, float y)  // Call once per frame
float OutputLeftValue           // Left motor output
float OutputRightValue          // Right motor output
int OutputOctant                // Active octant (0–8)
```

> **Note:** `OutputOctant` returns `int` directly to keep octant conditionals type-safe across Unity algorithm variants.

## What `_octant` Is Used For

`_octant` is the algorithm's discrete motion-state label (0 to 8) for the current joystick input.

- **0** = center dead zone / stop
- **1-8** = directional regions used by the branch logic

`OutputOctant` lets caller code make clean integer conditionals for:

- kinematics mode selection in simulation
- behavior/state switching (stop, forward-curve, pivot, reverse-curve)
- debugging and telemetry (confirming which region is active)

---

## Verification

Algorithm.cs was verified against `JoystickAlgorithm.cs` across a 20-point input test covering all 8 octants, the center stop, the tolerance boundary, and the diagonal boundary. **All 20 test cases matched exactly.** See the main `README.md` verification table for full results.

## Speed

Algorithm.cs averaged **6.6355 seconds** over 100,000,000 calls (5 runs × 1,000,000 iterations × 20 inputs). It is approximately **8.75% slower** than the compact refactor due to repeated `Mathf.Abs` calls on every branch evaluation.

---

## Relationship to Other Files

| File | Relationship |
|------|-------------|
| `MCU-Original/Joystick.h` | The original C++ implementation this file corrects — same octant logic, floating-point equality bugs removed |
| `Unity-Compact/JoystickAlgorithm.cs` | Compact refactor of this file — identical outputs, caches absX/absY, uses nested ternaries, ~8.75% faster |
| `MCU-Compact/Joystick.h` | C++ port of the compact logic — derived from `JoystickAlgorithm.cs`, not directly from this file |

---

> **Status:** ✅ Verified correct and functionally equivalent to `JoystickAlgorithm.cs`. 
> Recommended for reference and educational purposes to understand the algorithm revision process. 
> For production use, prefer `Unity-Compact/JoystickAlgorithm.cs` due to its 8.75% performance improvement. 
> Both are maintained in the MageMCU Unity repositories, with Robot-6 as the Unity Asset Store target.

---

## Quality Assurance — 20-Point Test Results

Tested against the same 20-point input set used throughout this study (June 25, 2026).

**20 of 20 points pass.**

Outputs are identical to `Unity-Compact/JoystickAlgorithm.cs` and `MCU-Compact/Joystick.h`
across all 20 test points — all 8 octants, center stop, tolerance boundary, and the
diagonal boundary where both axes are equal. No divergence found at any point.

Point 02 (X=0.0005, Y=0.0005) correctly returns `Left=0.0000, Right=0.0000` — the
tolerance dead zone absorbs the near-zero input and produces a clean motor stop,
confirming the fix over MCU-Original.

*Full results: see `QA-Report.md` in the `Joystick-Algorithm-Study/` folder.*
