#ifndef QUICKPID_EMBEDDED_H
#define QUICKPID_EMBEDDED_H

#include <cstdint>

class QuickPID
{
public:
    enum class Action : uint8_t {direct, reverse};
    enum class pMode : uint8_t {pOnError, pOnMeas, pOnErrorMeas};
    enum class dMode : uint8_t {dOnError, dOnMeas};
    enum class iAwMode : uint8_t {iAwCondition, iAwClamp, iAwOff};
    QuickPID(float* Kp,  float* Ki, float* Kd, float* dt, float* maxPoint, float* minPoint, float* setPoint, float* value, float* prev);
    float calculate();
    void null();
    inline void setAction(Action action) {_action = action;}
    inline void setPmode(pMode pmode) {_pmode = pmode;}
    inline void setDmode(dMode dmode) {_dmode = dmode;}
    inline float constrain(float value, float maxValue, float minValue) {if(value > maxValue) {value = maxValue;} if(value < minValue) {value = minValue;} return value;};


private:
    float* _Kp {nullptr};
    float* _Ki {nullptr};
    float* _Kd {nullptr};
    float* _dt {nullptr};
    float* _maxPoint {nullptr};
    float* _minPoint {nullptr};
    float*_setPoint {nullptr};
    float*_value {nullptr};
    float* _prev{nullptr};
    float _dInput {0.0}, _dError {0.0};
    float _peTerm {0.0}, _pmTerm {0.0};
    float _pTerm{0.0}, _iTerm {0.0}, _dTerm {0.0}, _error {0.0}, _ePrev{0.0}, _output{0.0}, _iTermOut{0.0};
    Action _action;
    pMode _pmode;
    dMode _dmode;
    iAwMode _iawmode;
};

QuickPID::QuickPID(float* Kp, float* Ki, float* Kd, float* dt, float* maxPoint, float* minPoint, float* setPoint, float* value, float* prev) :
    _Kp(Kp), _Ki(Ki), _Kd(Kd), _dt(dt), _maxPoint(maxPoint), _minPoint(minPoint), _setPoint(setPoint), _value(value), _prev(prev)
{
    _action = Action::reverse;
    _pmode = pMode::pOnErrorMeas;
    _dmode = dMode::dOnError;
    _iawmode = iAwMode::iAwCondition;
    //_output = 0.0;
}

float QuickPID::calculate()
{
    float value = *_value;
    float prev = *_prev;
    float setPoint = *_setPoint;
    //float dt = *_dt;
    float Kp = *_Kp;
    float Ki = *_Ki;
    float Kd = *_Kd;
    //Ki *= (dt/1000);
    //Kd /= (dt/1000);
    _dInput = value - prev;
    if(_action == Action::reverse) _dInput = -_dInput;
    _error = setPoint - value;
    if(_action == Action::reverse) _error = -_error;
    _dError = _error - _ePrev;
    _peTerm = Kp * _error;
    _pmTerm = Kp * _dInput;
    if (_pmode == pMode::pOnError) _pmTerm = 0;
    else
        if (_pmode == pMode::pOnMeas) _peTerm = 0;
        else
        { //pOnErrorMeas
            _peTerm *= 0.5f;
            _pmTerm *= 0.5f;
        }
    _pTerm = _peTerm - _pmTerm;
    _iTerm = Ki * _error;
    _iTermOut += _iTerm;
    if (_dmode == dMode::dOnError) _dTerm = *_Kd * _dError;
    else _dTerm = -Kd * _dInput;
    /*
    if (_iawmode == iAwMode::iAwCondition)
    {
        bool aw = false;
        if (_iTermOut > *_maxPoint && _dError > 0.0f) aw = true;
        else if (_iTermOut < *_maxPoint && _dError < 0.0f) aw = true;
        float maxPoint = *_maxPoint;
        float minPoint = *_minPoint;
        if (aw && *_Ki) _iTermOut = constrain(_iTermOut, maxPoint, minPoint);
    }
    */
    _output += _pTerm;
    _output += _iTermOut;
    _output += _dTerm;
    float maxPoint = *_maxPoint;
    float minPoint = *_minPoint;
    _output = constrain( _output, maxPoint, minPoint);
    return _output;
}

void QuickPID::null()
{
    _iTermOut = 0.0;
}

#endif // QUICKPID_EMBEDDED_H
