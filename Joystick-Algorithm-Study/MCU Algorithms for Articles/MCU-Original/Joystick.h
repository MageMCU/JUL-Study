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
//  * MCU:Atmega328P
//  * IDE:PlatformIO
//  * Editor: VSCode
//
// MIT LICENSE
//

#ifndef Articles_MCU_Original_Joystick_h
#define Articles_MCU_Original_Joystick_h

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
    };

    // Constructor
    template <typename real>
    Algorithm<real>::Algorithm()
    {
        _inputX = (real)0;
        _inputY = (real)0;
        _outputLeft = (real)0;
        _outputRight = (real)0;
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

    // Private Method
    template <typename real>
    void Algorithm<real>::_joystick()
    {
        real outLeft = (real)0;
        real outRight = (real)0;

        real delta = absT<real>(absT<real>(_inputY) - absT<real>(_inputX));

        // Quadrant 1
        if ((_inputX > (real)0) && (_inputY > (real)0))
        {
            if (absT<real>(_inputX) >= absT<real>(_inputY))
            {
                // Within Octant 1 (++)
                // x-dominate
                // POS      POS
                outLeft = _inputX;
                // NEG      -POS
                outRight = -delta;
            }
            else if (absT<real>(_inputX) < absT<real>(_inputY))
            {
                // Within Octant 2 (++)
                // y-dominate
                // POS      POS
                outLeft = _inputY;
                // POS      POS
                outRight = delta;
            }
        } // Quadrant 2
        else if ((_inputX < (real)0) && (_inputY > (real)0))
        {
            if (absT<real>(_inputX) < absT<real>(_inputY))
            {
                // Within Octant 3 (-+)
                // y-dominate
                // POS      POS
                outLeft = delta;
                // POS      POS
                outRight = _inputY;
            }
            else if (absT<real>(_inputX) >= absT<real>(_inputY))
            {
                // Octant 4 (-+)
                // x-dominate
                // NEG      -POS
                outLeft = -delta;
                // POS      -NEG
                outRight = -_inputX;
            }
        } // Quadrant 3
        else if ((_inputX < (real)0) && (_inputY < (real)0))
        {
            if (absT<real>(_inputX) >= absT<real>(_inputY))
            {
                // Octant 5 (--)
                // x-dominate
                // NEG      NEG
                outLeft = _inputX;
                // POS      POS
                outRight = delta;
            }
            else if (absT<real>(_inputX) < absT<real>(_inputY))
            {
                // Octant 6 (--)
                // y-dominate
                // NEG      NEG
                outLeft = _inputY;
                // NEG      -POS
                outRight = -delta;
            }
        } // Quadrant 4
        else if ((_inputX > (real)0) && (_inputY < (real)0))
        {
            if (absT<real>(_inputX) < absT<real>(_inputY))
            {
                // Octant 7 (+-)
                // y-dominate
                // NEG      -POS
                outLeft = -delta;
                // NEG      NEG
                outRight = _inputY;
            }
            else if (absT<real>(_inputX) >= absT<real>(_inputY))
            {
                // Octant 8 (+-)
                // x-dominate
                // POS      POS
                outLeft = delta;
                // NEG      -POS
                outRight = -_inputX;
            }
        } // Forward & Backward
        else if ((_inputX == (real)0) && (_inputY != (real)0))
        {
            outLeft = _inputY;
            outRight = _inputY;
        } // Left & Right Turns
        else if ((_inputX != (real)0) && (_inputY == (real)0))
        {
            outLeft = _inputX;
            outRight = -_inputX;
        } // STOP - Requires OFFSET
        else if ((_inputX == (real)0) && (_inputY == (real)0))
        {
            outLeft = 0;
            outRight = 0;
        }

        _outputLeft = outLeft;
        _outputRight = outRight;
    }
}
#endif
