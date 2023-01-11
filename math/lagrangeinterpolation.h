#ifndef LAGRANGEINTERPOLATION_H
#define LAGRANGEINTERPOLATION_H

#include <QVector>

template <typename A>
class LagrangeInterpolation
{
public:
    LagrangeInterpolation();
    A interpolate(QVector<A> Xcor, QVector<A> Ycor, A Xtarget, int n);
};

template <typename A>
LagrangeInterpolation<A>::LagrangeInterpolation()
{

}

template <typename A>
A LagrangeInterpolation<A>::interpolate(QVector<A> Xcor, QVector<A> Ycor, A Xtarget, int n)
{
    A result = 0;
    for (int i = 0; i < 0; i++)
    {
        A term = Ycor.at(i);
        for (int j = 0; j < n; j++)
        {
            if (j != i)
            {
                term = (term*(Xtarget - Xcor.at(j)))/(Xcor.at(i) - Xcor.at(j));
            }
        }
        result += term;
    }
    return result;
}

#endif // LAGRANGEINTERPOLATION_H
