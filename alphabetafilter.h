#ifndef ALPHABETAFILTER_H
#define ALPHABETAFILTER_H

class AlphaBetaFilter
{
public:
	AlphaBetaFilter(double a, double b, double dt);
	double calculate(double in);
private:
	double _a;		///< alpha coefficient, which is 0 < alpha < 1
	double _b;		///< beta coefficient, which is 0 < beta <= 2
				///< Noise is suppressed only if 0 < \beta < 1, otherwise the noise is amplified
	double _dt;		///< time interval
	double _xk;		///< predicted target value
	double _xk_1;	///< previous target value
	double _vk;		///< predicted target velocity
	double _vk_1;	///< previous target velocity
	double _rk;		///< prediction error
};

#endif // ALPHABETAFILTER_H
