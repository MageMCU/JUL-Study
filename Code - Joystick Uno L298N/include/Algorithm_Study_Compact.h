//
// Carpenter Software
// File: Class Joystick.h
// Github: MageMCU
// Repository: Joystick-UNO-L298N
// Folder: Code
//
// By Jesse Carpenter (carpentersoftware.com)
//
// Testing Platform:
//  * MCU: Atmega328P
//  * IDE: PlatformIO
//  * Editor: VSCode
//
// MIT LICENSE
//
// NOTE: This implementation ports the compact Unity joystick algorithm from
// Unity-Compact/JoystickAlgorithm.cs into the Arduino Uno class structure. It
// keeps the same tolerance stop, cached absolute values, and compact octant
// routing used by the Unity study version.
//
// NOTE: Hardware testing on Arduino Uno with L298N motor driver is complete.
// Algorithm output verified against MCU-Original across all 8 octants.
// See MCU-Compact/README.md and QA-Report.md for full results.
//

#ifndef Code_JUL_Algorithm_Compact_h
#define Code_JUL_Algorithm_Compact_h

#include <Arduino.h>
#include "MiscMath.h"
#include "Common.h"

// Carpenter Software - Jesse Carpenter
namespace csjc
{
    template <typename real>
    class Algorithm
    {
    private:
        // PROPERTIES
        // INPUT
        real _inputX;
        real _inputY;
        // OUTPUT
        real _outputLeft;
        real _outputRight;
        // OCTANT
        int _octant;
        // METHODS
        void _joystick();

    public:
        // Constructor
        Algorithm();
        ~Algorithm() = default;
        // Getters
        bool IsLeftForward();
        bool IsRightForward();
        // METHODS
        void UpdateInputs(real inputX, real inputY);
        real Left();
        real Right();
        int Octant();
    };

    // Constructor
    template <typename real>
    Algorithm<real>::Algorithm()
    {
        _inputX = (real)0;
        _inputY = (real)0;
        _outputLeft = (real)0;
        _outputRight = (real)0;
        _octant = 0;
    }

    // Getters
    template <typename real>
    bool Algorithm<real>::IsLeftForward()
    {
        return _outputLeft > (real)0;
    }

    template <typename real>
    bool Algorithm<real>::IsRightForward()
    {
        return _outputRight > (real)0;
    }

    // Method update
    template <typename real>
    void Algorithm<real>::UpdateInputs(real inputX, real inputY)
    {
        _inputX = inputX;
        _inputY = inputY;
        _joystick();
    }

    // Method output
    template <typename real>
    real Algorithm<real>::Left()
    {
        return _outputLeft;
    }

    // Method output
    template <typename real>
    real Algorithm<real>::Right()
    {
        return _outputRight;
    }

    // Method output
    template <typename real>
    int Algorithm<real>::Octant()
    {
        return _octant;
    }

    // Private Method
    template <typename real>
    void Algorithm<real>::_joystick()
    {
        // Tolerance defines the dead zone radius around the stick center.
        // Both axes must fall below this threshold to trigger a full stop.
        // Replaces the original floating-point equality checks for STOP,
        // Forward/Backward, and Left/Right, which fail under ADC noise.
        const real _tolerance = (real)0.001;

        // Local motor values, initialized to zero (safe default).
        real outLeft = (real)0;
        real outRight = (real)0;

#ifdef DEBUG_JOYSTICK
        Debug("Joystick INPUT", _inputX, _inputY);
#endif

        // Cache absolute values once to avoid repeated absT calls
        // throughout the octant logic. This is the key efficiency gain
        // over the original implementation.
        real absX = absT<real>(_inputX);
        real absY = absT<real>(_inputY);

        // Delta is the magnitude difference between the two axis values.
        // Using cached absX and absY simplifies the expression compared
        // to the double absT form in the original, with identical results.
        real delta = absT<real>(absX - absY);

        // --- OCTANT ZERO: CENTER STOP ---
        if (absX < _tolerance && absY < _tolerance)
        {
            _octant = 0;
#ifdef DEBUG_JOYSTICK
            Debug("STOP", outLeft, outRight);
#endif
        }
        // --- X-DOMINANT OCTANTS (1, 4, 5, 8) ---
        else if (absX >= absY)
        {
            _octant = (_inputX >= (real)0) ? ((_inputY >= (real)0) ? 1 : 8)
                                           : ((_inputY >= (real)0) ? 4 : 5);
            if (_inputY >= (real)0)
            {
                // Upper half (Y >= 0): Octants 1 and 4
                outLeft  = _inputX >= (real)0 ?  _inputX : -delta;
                outRight = _inputX >= (real)0 ? -delta   : -_inputX;
#ifdef DEBUG_JOYSTICK
                Debug(_inputX >= (real)0 ? "Octant-1" : "Octant-4", outLeft, outRight);
#endif
            }
            else
            {
                // Lower half (Y < 0): Octants 5 and 8
                outLeft  = _inputX >= (real)0 ?  delta   :  _inputX;
                outRight = _inputX >= (real)0 ? -_inputX :  delta;
#ifdef DEBUG_JOYSTICK
                Debug(_inputX >= (real)0 ? "Octant-8" : "Octant-5", outLeft, outRight);
#endif
            }
        }
        // --- Y-DOMINANT OCTANTS (2, 3, 6, 7) ---
        else
        {
            _octant = (_inputY >= (real)0) ? ((_inputX >= (real)0) ? 2 : 3)
                                           : ((_inputX >= (real)0) ? 7 : 6);
            if (_inputX >= (real)0)
            {
                // Right half (X >= 0): Octants 2 and 7
                outLeft  = _inputY >= (real)0 ?  _inputY : -delta;
                outRight = _inputY >= (real)0 ?  delta   :  _inputY;
#ifdef DEBUG_JOYSTICK
                Debug(_inputY >= (real)0 ? "Octant-2" : "Octant-7", outLeft, outRight);
#endif
            }
            else
            {
                // Left half (X < 0): Octants 3 and 6
                outLeft  = _inputY >= (real)0 ?  delta   :  _inputY;
                outRight = _inputY >= (real)0 ?  _inputY : -delta;
#ifdef DEBUG_JOYSTICK
                Debug(_inputY >= (real)0 ? "Octant-3" : "Octant-6", outLeft, outRight);
#endif
            }
        }

        _outputLeft = outLeft;
        _outputRight = outRight;
    }
}
#endif
