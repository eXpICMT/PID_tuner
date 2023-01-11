#ifndef COHENCOON_H
#define COHENCOON_H

#include <vector>
#include <algorithm>
#include <QDebug>

using namespace std;

static constexpr int WINDOW_SIZE = 4; //should be even!!!
static constexpr int COUNTER_SIZE = 50; //[cycles]
static constexpr int PERIOD_SIZE = 1000; //[ms]

//The sliding filter of tracking of curve's slope (SFTCS)
class SFTCS
{
public:
/*
    vector<float>measValue;
    vector<int>measDt;
    vector<float>slope;
    vector<float>tangent;
*/
    void setMeasValue(float value);
    inline void setMeasDt(int* dt) {measPeriod = dt;};
    inline void nullDt() {dTcounter = 0;}
    inline void setTprev(int* Tprev) {_Tprev = Tprev;}
    inline void setTact(int* Tact) {_Tact = Tact;}

    inline float getMaxSlope() const {return maxSlope;}
    float maxSlope {0.0}, maxVprev {0.0}, maxVact {0.0};
    int maxTprev {0}, maxTact {0};
    float prevMeasValue {0.0}, actualV {0.0}, delta {0.0};
//private:
    int first {1};
    int dTcounter {0};//[ms]
    int* measPeriod {nullptr}, *_Tprev {nullptr}, *_Tact {nullptr};

};

void SFTCS::setMeasValue(float value)
{
    if(!first)
    {
        delta = value - prevMeasValue;
        if(delta >= 0.0f)
        {
            dTcounter += *measPeriod;
        }
    }
    if((dTcounter >= PERIOD_SIZE) || first) //one time per for around 1000 ms
    {
        actualV = value;
        first = 0;
        dTcounter = 0.0f;
        if(delta > maxSlope)
        {
            maxVprev = prevMeasValue;
            maxVact = actualV;
            maxTprev = *_Tprev;
            maxTact = *_Tact;
            maxSlope = delta;
        }
    }
    prevMeasValue = value;
}

#endif // COHENCOON_H
