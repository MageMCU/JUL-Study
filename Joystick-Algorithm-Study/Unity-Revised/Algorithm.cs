// Carpenter Software - Jesse Carpenter (carpentersoftware.com)
// MageMCU - github.com/MageMCU
//
// Algorithm.cs
// Purpose: Maps a two-axis joystick input to left and right motor output
// values for a differential drive robot. The joystick's square input space
// is divided into nine regions: a center stop zone (Octant 0) and eight
// surrounding octants. The dominant axis determines robot speed; the delta
// between the two axes determines the motor differential (turning rate).
//
// Inputs:  inputX, inputY  — normalized floats in the range -1.0 to 1.0
// Outputs: OutputLeftValue, OutputRightValue — motor drive values (-1.0 to 1.0)
//          OutputOctant — integer (0–8) identifying the active joystick region

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

        // Initialize resets all inputs and octant to a known stopped state.
        // Call this before the first Update() to ensure clean startup.
        public void Initialize()
        {
            _inputX = 0f;
            _inputY = 0f;
            _octant = 0;
        }

        // Read-only properties expose the processed outputs to the caller.
        public float OutputLeftValue { get { return _outputLeftValue; } }
        public float OutputRightValue { get { return _outputRightValue; } }
        public int OutputOctant { get { return _octant; } }

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
            // Any input with both axes below this threshold is treated as stop.
            // This prevents motor chatter from analog noise at rest position.
            const float _tolerance = 0.001f;

            // Local motor values, initialized to zero (safe default).
            float leftMotorValue = 0f;
            float rightMotorValue = 0f;

            // Delta is the magnitude difference between the two axis values.
            // It represents how far the joystick is from the diagonal (45-degree)
            // line between the two axes. Delta drives the motor differential —
            // a delta of 0 means both motors run equally; a larger delta means
            // one motor runs significantly faster or slower than the other.
            //
            // The double Mathf.Abs is intentional and load-bearing:
            //   inner Abs: ensures both operands are non-negative magnitudes
            //   outer Abs: ensures the subtraction result is always non-negative
            // Removing either Abs produces incorrect results when inputs carry
            // opposite signs across quadrants.
            float delta = Mathf.Abs(Mathf.Abs(_inputX) - Mathf.Abs(_inputY));

            // --- OCTANT ZERO: CENTER STOP ---
            // If both axes are within the tolerance dead zone, stop all motors.
            // This check runs first to prevent any octant branch from firing
            // when the stick is at rest.
            if ((Mathf.Abs(_inputX) < _tolerance) && (Mathf.Abs(_inputY) < _tolerance))
            {
                leftMotorValue = 0f;
                rightMotorValue = 0f;
                _octant = 0;
            }
            else
            {
                // --- X-DOMINANT OCTANTS (1, 4, 5, 8) ---
                // When the X axis magnitude equals or exceeds the Y axis magnitude,
                // the robot is turning harder than it is driving straight.
                // The dominant axis (X) sets the drive speed of the faster motor.
                // Delta sets the slower motor's output, producing the turn.
                if (Mathf.Abs(_inputX) >= Mathf.Abs(_inputY))
                {
                    if (_inputX >= 0f && _inputY >= 0f)
                    {
                        // Octant 1: Quadrant I, X-dominant (forward-right turn)
                        leftMotorValue = _inputX;
                        rightMotorValue = -delta;
                        _octant = 1;
                    }
                    else if (_inputX <= 0f && _inputY >= 0f)
                    {
                        // Octant 4: Quadrant II, X-dominant (forward-left turn)
                        leftMotorValue = -delta;
                        rightMotorValue = -_inputX;
                        _octant = 4;
                    }
                    else if (_inputX <= 0f && _inputY <= 0f)
                    {
                        // Octant 5: Quadrant III, X-dominant (reverse-left turn)
                        leftMotorValue = _inputX;
                        rightMotorValue = delta;
                        _octant = 5;
                    }
                    else if (_inputX >= 0f && _inputY <= 0f)
                    {
                        // Octant 8: Quadrant IV, X-dominant (reverse-right turn)
                        leftMotorValue = delta;
                        rightMotorValue = -_inputX;
                        _octant = 8;
                    }
                }
                // --- Y-DOMINANT OCTANTS (2, 3, 6, 7) ---
                // When the Y axis magnitude exceeds the X axis magnitude,
                // the robot is driving more straight than turning.
                // The dominant axis (Y) sets the drive speed of both motors.
                // Delta sets the differential, producing a gentle curve.
                else if (Mathf.Abs(_inputX) < Mathf.Abs(_inputY))
                {
                    if (_inputX >= 0f && _inputY >= 0f)
                    {
                        // Octant 2: Quadrant I, Y-dominant (forward, curving right)
                        leftMotorValue = _inputY;
                        rightMotorValue = delta;
                        _octant = 2;
                    }
                    else if (_inputX <= 0f && _inputY >= 0f)
                    {
                        // Octant 3: Quadrant II, Y-dominant (forward, curving left)
                        leftMotorValue = delta;
                        rightMotorValue = _inputY;
                        _octant = 3;
                    }
                    else if (_inputX <= 0f && _inputY <= 0f)
                    {
                        // Octant 6: Quadrant III, Y-dominant (reverse, curving left)
                        leftMotorValue = _inputY;
                        rightMotorValue = -delta;
                        _octant = 6;
                    }
                    else if (_inputX >= 0f && _inputY <= 0f)
                    {
                        // Octant 7: Quadrant IV, Y-dominant (reverse, curving right)
                        leftMotorValue = -delta;
                        rightMotorValue = _inputY;
                        _octant = 7;
                    }
                }
            }

            // Write computed values to output fields for caller access.
            _outputLeftValue = leftMotorValue;
            _outputRightValue = rightMotorValue;
        }
    }
};
