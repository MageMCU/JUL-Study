// Carpenter Software - Jesse Carpenter (carpentersoftware.com)
// MageMCU - github.com/MageMCU
//
// JoystickAlgorithm.cs  (Robot-8 Study / Code / Joystick)
// Purpose: Compact refactor of Algorithm.cs developed during a Robot-8 study.
// Note: Robot-8 is not part of the four published MageMCU Unity repositories.
// Maps a two-axis joystick input to left and right motor output values for
// a differential drive robot. Input space is divided into nine regions:
// a center stop zone (Octant 0) and eight directional octants.
//
// Key improvements over Algorithm.cs:
//   - absX and absY cached once, eliminating repeated Mathf.Abs calls
//   - delta simplified to Mathf.Abs(absX - absY) using cached magnitudes
//   - Octant assigned via nested ternary for compact quadrant resolution
//   - Explicit default constructor retained for parity with the study version
//
// Inputs:  inputX, inputY  — normalized floats in the range -1.0 to 1.0
// Outputs: Left, Right     — motor drive values (-1.0 to 1.0)
//          Octant          — integer (0–8) identifying the active joystick region

using UnityEngine;

namespace Joystick
{
    public class Algorithm
    {
        // Motor output values written by _process() and exposed via properties
        private float _outputLeftValue;
        private float _outputRightValue;

        // Joystick axis inputs stored from the last Update() call
        private float _inputX;
        private float _inputY;

        // Active octant number (0 = stop, 1–8 = directional regions)
        private int _octant;

        // Explicit default constructor — no initialization here;
        // call Initialize() before the first Update().
        public Algorithm() {}

        // Read-only properties expose processed outputs to the caller.
        // Property names are shortened from the original (Left/Right vs
        // OutputLeftValue/OutputRightValue) for cleaner caller code.
        // Octant returns int directly, correcting the float return type
        // present in the original Algorithm.cs.
        public float Left { get { return _outputLeftValue; } }
        public float Right { get { return _outputRightValue; } }
        public int Octant { get { return _octant; } }

        // Initialize() resets all inputs and octant to a known stopped state.
        // Call this before the first Update() to ensure clean startup.
        public void Initialize()
        {
            _inputX = 0f;
            _inputY = 0f;
            _octant = 0;
        }

        // Update() accepts the current joystick axis values and immediately
        // triggers _process() to compute new motor output values.
        // Call this once per frame from the MonoBehaviour Update loop.
        public void Update(float inputX, float inputY)
        {
            _inputX = inputX;
            _inputY = inputY;
            _process();
        }

        private void _process()
        {
            // Tolerance defines the dead zone radius around the stick center.
            // Both axes must fall below this threshold to trigger a full stop.
            // Prevents motor chatter from analog noise at the rest position.
            const float _tolerance = 0.001f;

            // Local motor values initialized to zero (safe default).
            float leftMotorValue = 0f;
            float rightMotorValue = 0f;

            // Cache absolute values once to avoid repeated Mathf.Abs calls
            // throughout the octant logic below. This is the key efficiency
            // gain over Algorithm.cs.
            float absX = Mathf.Abs(_inputX);
            float absY = Mathf.Abs(_inputY);

            // Delta is the magnitude difference between the two axis values.
            // Using cached absX and absY simplifies the expression compared
            // to the double Mathf.Abs form in Algorithm.cs, with identical results.
            float delta = Mathf.Abs(absX - absY);

            // --- OCTANT ZERO: CENTER STOP ---
            if (absX < _tolerance && absY < _tolerance)
            {
                _octant = 0;
            }
            // --- X-DOMINANT OCTANTS (1, 4, 5, 8) ---
            else if (absX >= absY)
            {
                _octant = (_inputX >= 0f) ? ((_inputY >= 0f) ? 1 : 8)
                                          : ((_inputY >= 0f) ? 4 : 5);

                if (_inputY >= 0f)
                {
                    // Upper half (Y >= 0): Octants 1 and 4
                    leftMotorValue  = _inputX >= 0f ?  _inputX : -delta;
                    rightMotorValue = _inputX >= 0f ? -delta   : -_inputX;
                }
                else
                {
                    // Lower half (Y < 0): Octants 5 and 8
                    leftMotorValue  = _inputX >= 0f ?  delta   :  _inputX;
                    rightMotorValue = _inputX >= 0f ? -_inputX :  delta;
                }
            }
            // --- Y-DOMINANT OCTANTS (2, 3, 6, 7) ---
            else
            {
                _octant = (_inputY >= 0f) ? ((_inputX >= 0f) ? 2 : 3)
                                          : ((_inputX >= 0f) ? 7 : 6);

                if (_inputX >= 0f)
                {
                    // Right half (X >= 0): Octants 2 and 7
                    leftMotorValue  = _inputY >= 0f ?  _inputY : -delta;
                    rightMotorValue = _inputY >= 0f ?  delta   :  _inputY;
                }
                else
                {
                    // Left half (X < 0): Octants 3 and 6
                    leftMotorValue  = _inputY >= 0f ?  delta   :  _inputY;
                    rightMotorValue = _inputY >= 0f ?  _inputY : -delta;
                }
            }

            // Write computed values to output fields for caller access.
            _outputLeftValue = leftMotorValue;
            _outputRightValue = rightMotorValue;
        }
    }
}
