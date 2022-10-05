#ifndef ALPHABETAFILTER_H
#define ALPHABETAFILTER_H

template<typename A, typename B>
class AlphaBetaFilter
{
public:
	AlphaBetaFilter(B a, B b, B dt);
	double calculate(A in);
private:
	B _a;		///< alpha coefficient, which is 0 < alpha < 1
	B _b;		///< beta coefficient, which is 0 < beta <= 2
				///< Noise is suppressed only if 0 < \beta < 1, otherwise the noise is amplified
	B _dt;		///< time interval
	A _xk;		///< predicted target value
	A _xk_1;	///< previous target value
	A _vk;		///< predicted target velocity
	A _vk_1;	///< previous target velocity
	A _rk;		///< prediction error
};

template<typename A, typename B>
AlphaBetaFilter<A, B>::AlphaBetaFilter(B a, B b, B dt):
	_a(a), _b(b), _dt(dt)
{
	_xk_1 = 0;
	_vk_1 = 0;
}

template<typename A, typename B>
double AlphaBetaFilter<A, B>::calculate(A in)
{
	_xk = _xk_1 + (_vk_1*_dt);
	_vk = _vk_1;
	_rk = in - _xk;
	_xk += _a*_rk;
	_vk += (_b*_rk)/_dt;
	_xk_1 = _xk;
	_vk_1 = _vk;
	return _xk;
}

#endif // ALPHABETAFILTER_H
