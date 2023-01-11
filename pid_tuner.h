#ifndef PID_TUNER_H
#define PID_TUNER_H

#include <cstdint>

class sTan
{
public:
    sTan();
    sTan(uint32_t bufSize) : _bufSize(bufSize) {}
    ~sTan() {};

    void begin(uint32_t bufSize);
    void init(float reading);
    float avgVal(float reading);
    float startVal();
    float slope(float reading);
    uint32_t length();

//private:
    uint32_t _bufSize;
    uint32_t _index;
    float _sum;
    float* _inArray;
};

void sTan::begin(uint32_t bufSize)
{
    _inArray = new float[bufSize];
    _bufSize = bufSize;
}

void sTan::init(float reading)
{
    _index = 0;
    _sum = reading * _bufSize;
    for (uint32_t i = 0; i < _bufSize; i++)
    {
        _inArray[i] = reading;
    }
}

float sTan::avgVal(float reading) {
    _index++;
    if (_index >= _bufSize) _index = 0;
    _sum += reading - _inArray[_index];
    _inArray[_index] = reading;
    return float (_sum / _bufSize);
}

float sTan::startVal() {
    uint16_t tailIndex = _index + 1;
    if (tailIndex >= _bufSize) tailIndex = 0;
    return _inArray[tailIndex];
}

float sTan::slope(float reading) {
    return reading - sTan::startVal();
}

uint32_t sTan::length() {
    return _bufSize;
}


class PidTuner
{
public:
    enum Action : uint8_t {directIP, direct5T, reverseIP, reverse5T};
    enum SerialMode : uint8_t {printOFF, printALL, printSUMMARY, printDEBUG};
    enum TunerStatus : uint8_t {sample, test, tunings, runPid, timerPid};
    enum TuningMethod : uint8_t { ZN_PID, DampedOsc_PID, NoOvershoot_PID, CohenCoon_PID, Mixed_PID};

    PidTuner();

};

sTan::sTan() {}
sTan stan = sTan();


/* FOR DEBUGGING S_TAN
    float temp {0.0};
    sTan stan(3);
    QVector<float>input {1, 2, 3, 4, 5, 6, 7, 8, 9};
    stan.begin(3);
    qDebug() << "stan.begin(3)";
    for(uint32_t i = 0; i < stan.length(); i++)
        qDebug() << "_inAr(" << i << ") = " << stan._inArray[i];
    //stan.init(input.at(0));
    qDebug() << "stan.init(input.at(0))";
    for(uint32_t i = 0; i < stan.length(); i++)
        qDebug() << "_inAr(" << i << ") = " << stan._inArray[i];
    for(uint32_t i = 0; i < input.size(); i++)
    {
        temp = stan.avgVal(input.at(i));
        qDebug() << " stan.avgVal(input.at(" << i << ")) = " << temp;
        qDebug() << "stan.index = " << stan._index;
        for(uint32_t i = 0; i < stan.length(); i++)
            qDebug() << "_inAr(" << i << ") = " << stan._inArray[i];
        temp = stan.slope(input.at(i));
        qDebug() << " stan.slope(input.at(" << i << ")) = " << temp;
    }
*/

#endif // PID_TUNER_H
