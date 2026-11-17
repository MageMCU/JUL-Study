# Unity-Compact — JoystickAlgorithm.cs

**Platform:** Unity Engine / C# (Robot-8 Study)
**Environment:** Robot-6 (Unity Asset Store pending)
**Author:** Jesse Carpenter (carpentersoftware.com) — MageMCU

---

## Purpose

`JoystickAlgorithm.cs` (class `Algorithm`) is the **compact refactor** of `Algorithm.cs`, developed during the Robot-8 advanced study. Robot-8 follows the same development direction as Robot-6 and is being used to apply Chapter 3 concepts from *Autonomous Mobile Robots* in a practical Unity setting. The octant logic and motor output values are **identical** to `Algorithm.cs`, but the implementation is tightened in three key ways:

1. `absX` and `absY` are cached once up front, eliminating repeated `Mathf.Abs` calls on every branch
2. `delta` is simplified to `Mathf.Abs(absX - absY)` using those cached values
3. Octant assignment uses nested ternary expressions instead of four separate conditional blocks

The result is fewer operations per frame with no change in behavior.

---

## How It Works

The algorithm divides the joystick input space into nine regions:

- **Octant 0 (center stop):** Both `absX` and `absY` within tolerance
- **X-dominant octants (1, 4, 5, 8):** `absX >= absY`
- **Y-dominant octants (2, 3, 6, 7):** `absX < absY`

The delta equation uses cached absolute values:

```csharp
float absX = Mathf.Abs(_inputX);
float absY = Mathf.Abs(_inputY);
float delta = Mathf.Abs(absX - absY);
```

This is mathematically identical to the double `Mathf.Abs` form in `Algorithm.cs` but requires one fewer `Mathf.Abs` call per evaluation because the magnitudes are already cached.

Octant assignment uses compact nested ternaries:

```csharp
// X-dominant
_octant = (_inputX >= 0f) ? ((_inputY >= 0f) ? 1 : 8)
                          : ((_inputY >= 0f) ? 4 : 5);
// Y-dominant
_octant = (_inputY >= 0f) ? ((_inputX >= 0f) ? 2 : 3)
                          : ((_inputX >= 0f) ? 7 : 6);
```

**Inputs:** `float inputX`, `float inputY` (normalized, range -1.0 to 1.0)
**Outputs:** `Left`, `Right` (range -1.0 to 1.0), `Octant` (0–8)

---

## Public API

```csharp
void Initialize()               // Call before first Update()
void Update(float x, float y)  // Call once per frame
float Left                      // Left motor output
float Right                     // Right motor output
int Octant                      // Active octant (0–8), correctly typed as int
```

> **Note:** Property names are shortened (`Left`/`Right` vs `OutputLeftValue`/`OutputRightValue`) for cleaner caller code. `Octant` returns `int` directly to keep octant-based conditionals explicit and type-safe.

## What `_octant` Is Used For

`_octant` is the compact algorithm's discrete region ID for the current joystick position.

- **0** = center dead zone / stop
- **1-8** = directional octants selected by the compact branch logic

The public `Octant` property is intended for integer-based conditionals in caller code, such as:

- simulation kinematics branching
- movement-state selection and control routing
- debug overlays and test validation by active region number

---

## Verification

Verified identical to `Algorithm.cs` across all 20 test points — all 8 octants, center stop, tolerance boundary, and diagonal boundary. **20/20 matches.** See the main `README.md` verification table.

## Speed

JoystickAlgorithm.cs averaged **6.0548 seconds** over 100,000,000 calls (5 runs × 1,000,000 iterations × 20 inputs) — approximately **8.75% faster** than `Algorithm.cs` (6.6355s avg). See the main `README.md` for the full speed comparison table, including the separate benchmark against `PolarCoordinate.cs`.

---

## Relationship to Other Files

| File | Relationship |
|------|-------------|
| `MCU-Original/Joystick.h` | Original C++ algorithm this study ultimately corrects — two generations removed |
| `Unity-Revised/Algorithm.cs` | Direct predecessor — this file is a verified equivalent compact refactor of Algorithm.cs |
| `MCU-Compact/Joystick.h` | Direct descendant — the compact logic here was ported back into C++ to produce the revised MCU header |

This file is the **primary recommended implementation** for Unity. The C++ port in `MCU-Compact/Joystick.h` was derived directly from this file's structure and logic.

---

> **Status:** ✅ Verified correct, fastest of all implementations, and the source for the MCU-Compact C++ port. This is the production-recommended algorithm for Unity simulation and the reference source behind the completed, hardware-validated Arduino revision.

---

## Quality Assurance — 20-Point Test Results

Tested against the same 20-point input set used throughout this study (June 25, 2026).

**20 of 20 points pass.**

Outputs are identical to `Unity-Revised/Algorithm.cs` and `MCU-Compact/Joystick.h`
across all 20 test points — all 8 octants, center stop, tolerance boundary, and the
diagonal boundary where both axes are equal. No divergence found at any point.

Point 02 (X=0.0005, Y=0.0005) correctly returns `Left=0.0000, Right=0.0000` — the
cached `absX`/`absY` and tolerance check produce a clean motor stop, confirming both
the correctness and the efficiency gain over MCU-Original.

*Full results: see `QA-Report.md` in the `Joystick-Algorithm-Study/` folder.*
