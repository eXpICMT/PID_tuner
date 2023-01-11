#ifndef SAMPLESTOPOINTS_H
#define SAMPLESTOPOINTS_H

#include <QVector>

template<typename A>
class SamplesToPoints
{
public:
    SamplesToPoints();
    QVector<A>* getInstance();
    bool sendNewDatum(A datum);
    A getXout();
    A getYout();
    QVector<A> pXoutQvector;    ///< X coordinate
    QVector<A> pYoutQvector;    ///< Y coordinate
    QVector<A> pFQvector;       ///< The input frequency of a value
private:
    enum STATES : bool
    {
        INPROCCESS      = false,
        READY           = true
    };
    //QVector<QVector<A>*> pXQvector;
    //QVector<QVector<A>*> pYQvector;


    A _temp;                    ///< The variable for temporary keeping a value
    A _prev;                    ///< The previous value of something
    A _outcome;
    A _x;
    A _y;
    A _delta;
    QVector<int> _n;
    int _counter{0}, _index{0};
    int _flag {0};
    bool _state;
};

template<typename A>
SamplesToPoints<A>::SamplesToPoints()
{
}

template<typename A>
QVector<A>* SamplesToPoints<A>::getInstance()
{
    static QVector<A> instance;
    return &instance;
}

template<typename A>
bool SamplesToPoints<A>::sendNewDatum(A datum)
{
    _outcome = datum;
    if(!_flag)
    {
        _prev = _outcome;
        _flag = 1;

    }
    _delta = _outcome - _prev;
    if(_delta == 0)
    {
        _counter++;
    }
    if(_delta > 0)
    {
        _index++;
        pFQvector.push_back(_counter);
        for (int i = 0; i < _index; i++)
        {
            _temp += pFQvector.at(i);
        }
        _temp -= pFQvector.at(_index - 1)/2;
        pXoutQvector.push_back(_temp);
        pYoutQvector.push_back(_prev);
        _temp = 0;
        _counter = 0;
        _counter++;
        _prev = _outcome;
        _state = READY;
    }
    else
    {
        _state = INPROCCESS;
    }
    return _state;
}

template<typename A>
A SamplesToPoints<A>::getXout()
{
    return pXoutQvector.at(_index - 1);
}

template<typename A>
A SamplesToPoints<A>::getYout()
{
    return pYoutQvector.at(_index - 1);
}

#endif // SAMPLESTOPOINTS_H
