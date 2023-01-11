/**********************************************************************************
   QuickPID Library for Arduino - Version 3.1.2
   by dlloydev https://github.com/Dlloydev/QuickPID
   Based on the Arduino PID_v1 Library. Licensed under the MIT License.
 **********************************************************************************/

#include "quickpid.h"

QuickPID::QuickPID()
{

}

float QuickPID::constrain(float Value, float MinValue, float MaxValue)
{
    if(Value < MinValue)
        Value = MinValue;
    if(Value > MaxValue)
        Value = MaxValue;
    return Value;
}

/* Compute() ***********************************************************************
   This function should be called every time "void loop()" executes. The function
   will decide whether a new PID Output needs to be computed. Returns true
   when the output is computed, false when nothing has been done.
 **********************************************************************************/
bool QuickPID::Compute() {

}

/* SetTunings(....)************************************************************
  This function allows the controller's dynamic performance to be adjusted.
  it's called automatically from the constructor, but tunings can also
  be adjusted on the fly during normal operation.
******************************************************************************/
void QuickPID::SetTunings(float Kp, float Ki, float Kd, float Period,
                          pMode pMode = pMode::pOnError,
                          dMode dMode = dMode::dOnMeas,
                          iAwMode iAwMode = iAwMode::iAwCondition) {

    if (Kp < 0 || Ki < 0 || Kd < 0) return;
    if (Ki == 0) outputSum = 0;
    pmode = pMode; dmode = dMode; iawmode = iAwMode;
    dispKp = Kp; dispKi = Ki; dispKd = Kd;
    kp = Kp;
    ki = Ki * (Period*1000);
    kd = Kd / (Period*1000);
}

/* SetOutputLimits(..)********************************************************
  The PID controller is designed to vary its output within a given range.
  By default this range is 0-255, the Arduino PWM range.
******************************************************************************/
void QuickPID::SetOutputLimits(float Min, float Max) {
    if (Min >= Max) return;
    outMin = Min;
    outMax = Max;

    if (mode != Control::manual) {
        *myOutput = constrain(*myOutput, outMin, outMax);
        outputSum = constrain(outputSum, outMin, outMax);
    }
}

/* SetMode(.)*****************************************************************
  Sets the controller mode to manual (0), automatic (1) or timer (2)
  when the transition from manual to automatic or timer occurs, the
  controller is automatically initialized.
******************************************************************************/
void QuickPID::SetMode(Control Mode) {
    if (mode == Control::manual && Mode != Control::manual) { // just went from manual to automatic or timer
        QuickPID::Initialize();
    }
    mode = Mode;
}

/* Initialize()****************************************************************
  Does all the things that need to happen to ensure a bumpless transfer
  from manual to automatic mode.
******************************************************************************/
void QuickPID::Initialize() {
    outputSum = *myOutput;
    lastInput = *myInput;
    outputSum = constrain(outputSum, outMin, outMax);
}
