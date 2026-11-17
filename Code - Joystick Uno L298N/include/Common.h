//
// Carpenter Software
// File: Class Common.h
// Github: MageMCU
// Repository: Joystick-UNO-L298N
// Folder: Code
//
// By Jesse Carpenter (carpentersoftware.com)
//
// Testing Platform:
//  * MCU:Atmega328P
//  * IDE:PlatformIO
//  * Editor: VSCode
//
// MIT LICENSE
//

#ifndef Common_h
#define Common_h

// ======================================================
// ALGORITHM SELECTION — PRIMARY MECHANISM
// Uncomment ONE line only.
// See README: "Testing Study Algorithms Without Replacing Joystick.h"
// Note: ALGO_ORIGINAL is informational only — the #else branch in
// main.cpp includes Joystick.h when neither study flag is defined.
// Alternatively, set -DALGO_STUDY_REVISED or -DALGO_STUDY_COMPACT
// in platformio.ini build_flags to override from the build system.
// ======================================================
#define ALGO_ORIGINAL
// #define ALGO_STUDY_REVISED
// #define ALGO_STUDY_COMPACT

// ======================================================
// DEBUG SELECTION
// Uncomment a module to enable its serial debug output.
// DEBUG_SERIAL_ON activates automatically — do not set it manually.
// NOTE: Enabling DEBUG_JOYSTICK also sets BUTTON_TIMER_mS to 3000ms.
//       Comment it out for normal (100ms) motor control loop speed.
// ======================================================
// #define DEBUG_MAIN
#ifdef DEBUG_MAIN
#define DEBUG_SERIAL_ON
#endif
// ------------------------------------------------------
// #define DEBUG_JOYSTICK
#ifdef DEBUG_JOYSTICK
#define DEBUG_SERIAL_ON
#endif
// ------------------------------------------------------
// #define DEBUG_L298N
#ifdef DEBUG_L298N
#define DEBUG_SERIAL_ON
#endif

// ======================================================
// TIMER
// Automatically set based on debug state.
// ======================================================
#ifdef DEBUG_SERIAL_ON
#define BUTTON_TIMER_mS 3000
#else
#define BUTTON_TIMER_mS 100
#endif

#endif // Common_h
