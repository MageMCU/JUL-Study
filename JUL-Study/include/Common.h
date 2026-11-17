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
// ALGORITHM SELECTION
// Uncomment ONE line only.
// See README: "Testing Study Algorithms Without Replacing Joystick.h"
// ======================================================
#define ALGO_ORIGINAL
// #define ALGO_STUDY_REVISED
// #define ALGO_STUDY_COMPACT
// Comment ALL To VERIFY Other Original

// ======================================================
// DEBUG SELECTION
// Uncomment a module to enable its serial debug output.
// DEBUG_SERIAL_ON activates automatically — do not set it manually.
// ======================================================
// #define DEBUG_MAIN
#ifdef DEBUG_MAIN
#define DEBUG_SERIAL_ON
#endif
// ------------------------------------------------------
#define DEBUG_JOYSTICK
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
