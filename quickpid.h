#ifndef QUICKPID_H
#define QUICKPID_H

#include <cstdint>

///< Derived from Dlloydev -> https://github.com/Dlloydev/QuickPID

class QuickPID
{
public:
    enum class Control : uint8_t
    {
        manual,
        automatic,
        timer
    };
    enum class Action : uint8_t
    {
        direct,
        reverse
    };
    enum class pMode : uint8_t
    {
        pOnError,
        pOnMeas,
        pOnErrorMeas
    };
    enum class dMode : uint8_t
    {
        dOnError,
        dOnMeas
    };
    enum class iAwMode : uint8_t
    {
        iAwCondition,
        iAwClamp,
        iAwOff
    };
    QuickPID();
    float constrain(float Value, float MinValue, float MaxValue);
    void SetMode(Control Mode);
    bool Compute();
    void SetOutputLimits(float Min, float Max);
    void SetTunings(float Kp, float Ki, float Kd, float Period, pMode pMode, dMode dMode, iAwMode iAwMode);
    inline void SetControllerDirection(Action Action) {action = Action;};
    inline void SetProportionalMode(pMode pMode) {pmode = pMode;};
    inline void SetDerivativeMode(dMode dMode) {dmode = dMode;};
    inline void SetAntiWindupMode(iAwMode iAwMode) {iawmode = iAwMode;};
    inline void SetInput(float* Input) {myInput = Input;};
    inline void SetOutput(float* Output)  {myOutput = Output;};
    inline void SetPeriod(float* Period) {myPeriod = Period;};
    inline void SetLastInput(float* LastInput) {myLastInput = LastInput;};

    inline float GetKp() {return dispKp;};                          // proportional gain
    inline float GetKi() {return dispKi;};                          // integral gain
    inline float GetKd() {return dispKd;};                          // derivative gain
    inline float GetPterm() {return pTerm;};                        // proportional component of output
    inline float GetIterm() {return iTerm;};                        // integral component of output
    inline float GetDterm() {return dTerm;};                        // derivative component of output
    inline uint8_t GetMode() {return static_cast<uint8_t>(mode);};              // manual (0), automatic (1) or timer (2)
    inline uint8_t GetDirection() {return static_cast<uint8_t>(action);};       // direct (0), reverse (1)
    inline uint8_t GetPmode() {return static_cast<uint8_t>(pmode);};            // pOnError (0), pOnMeas (1), pOnErrorMeas (2)
    inline uint8_t GetDmode() {return static_cast<uint8_t>(dmode);};            // dOnError (0), dOnMeas (1)
    inline uint8_t GetAwMode() {return static_cast<uint8_t>(iawmode);};         // iAwCondition (0, iAwClamp (1), iAwOff (2)

private:

    void Initialize();

    float dispKp = 0;   // for defaults and display
    float dispKi = 0;
    float dispKd = 0;
    float pTerm;
    float iTerm;
    float dTerm;

    float kp;           // (P)roportional Tuning Parameter
    float ki;           // (I)ntegral Tuning Parameter
    float kd;           // (D)erivative Tuning Parameter

    float *myInput;     // Pointers to the Input, Output, and Setpoint variables. This creates a
    float *myOutput;    // hard link between the variables and the PID, freeing the user from having
    float *mySetpoint;  // to constantly tell us what these values are. With pointers we'll just know.
    float *myPeriod;
    float *myLastInput;

    Control mode = Control::manual;
    Action action = Action::direct;
    pMode pmode = pMode::pOnError;
    dMode dmode = dMode::dOnMeas;
    iAwMode iawmode = iAwMode::iAwCondition;

    uint32_t sampleTimeUs, lastTime, period;
    float outputSum, outMin, outMax, error, lastError, lastInput;
};

#endif // QUICKPID_H
