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
// NOTE: This implementation ports the revised Unity joystick algorithm into the
// Arduino Uno class structure. It preserves the original public API while
// matching the tolerance stop, delta equation, and octant routing used in the
// Unity study implementation.
//

#ifndef Articles_MCU_Revised_Joystick_h
#define Articles_MCU_Revised_Joystick_h

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
        // Active octant number (0 = stop, 1-8 = directional regions)
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
        const real _tolerance = (real)0.001;

        // Local motor values, initialized to zero (safe default).
        real outLeft = (real)0;
        real outRight = (real)0;

        // Delta matches the revised Unity form:
        // abs(abs(x) - abs(y))
        real delta = absT<real>(absT<real>(_inputX) - absT<real>(_inputY));

        // --- OCTANT ZERO: CENTER STOP ---
        if (absT<real>(_inputX) < _tolerance && absT<real>(_inputY) < _tolerance)
        {
            outLeft = (real)0;
            outRight = (real)0;
            _octant = 0;
        }
        else
        {
            // --- X-DOMINANT OCTANTS (1, 4, 5, 8) ---
            if (absT<real>(_inputX) >= absT<real>(_inputY))
            {
                if (_inputX >= (real)0 && _inputY >= (real)0)
                {
                    outLeft = _inputX;
                    outRight = -delta;
                    _octant = 1;
                }
                else if (_inputX <= (real)0 && _inputY >= (real)0)
                {
                    outLeft = -delta;
                    outRight = -_inputX;
                    _octant = 4;
                }
                else if (_inputX <= (real)0 && _inputY <= (real)0)
                {
                    outLeft = _inputX;
                    outRight = delta;
                    _octant = 5;
                }
                else if (_inputX >= (real)0 && _inputY <= (real)0)
                {
                    outLeft = delta;
                    outRight = -_inputX;
                    _octant = 8;
                }
            }
            // --- Y-DOMINANT OCTANTS (2, 3, 6, 7) ---
            else if (absT<real>(_inputX) < absT<real>(_inputY))
            {
                if (_inputX >= (real)0 && _inputY >= (real)0)
                {
                    outLeft = _inputY;
                    outRight = delta;
                    _octant = 2;
                }
                else if (_inputX <= (real)0 && _inputY >= (real)0)
                {
                    outLeft = delta;
                    outRight = _inputY;
                    _octant = 3;
                }
                else if (_inputX <= (real)0 && _inputY <= (real)0)
                {
                    outLeft = _inputY;
                    outRight = -delta;
                    _octant = 6;
                }
                else if (_inputX >= (real)0 && _inputY <= (real)0)
                {
                    outLeft = -delta;
                    outRight = _inputY;
                    _octant = 7;
                }
            }
        }

        _outputLeft = outLeft;
        _outputRight = outRight;
    }
}
#endif
