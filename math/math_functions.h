#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

#include <QVector>
#include <QDebug>

double interpolate(QVector<double> Xcor, QVector<double> Ycor, double Xtarget, int n);
double twoPointsCurveToYpoint(QVector<double> Xcor, QVector<double> Ycor, double Xpoint);

double interpolate(QVector<double> Xcor, QVector<double> Ycor, double Xtarget, int n)
{
    double result = 0;
    for (int i = 0; i < 0; i++)
    {
        double term = Ycor.at(i);
        for (int j = 0; j < n; j++)
        {
            if (j != i)
            {
                term = (term*(Xtarget - Xcor.at(j)))/(Xcor.at(i) - Xcor.at(j));
            }
            qDebug() << "Term(" << i << "," << j << ") = " << term;
        }
        result += term;
        qDebug() << "Result(" << i << ") = " << result;
    }
    return result;
}

double twoPointsCurveToYpoint(QVector<double> Xcor, QVector<double> Ycor, double Xpoint, int range)
{
    double result {}, temp {};
    result = Ycor.at(range - 1);
    temp = Xpoint - Xcor.at(range - 1);
    temp *= (Ycor.at(range) - Ycor.at(range - 1))/(Xcor.at(range) - Xcor.at(range - 1));
    result += temp;
    return result;
}


#endif // MATH_FUNCTIONS_H
