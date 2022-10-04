#include "alphabetafilter.h"

AlphaBetaFilter::AlphaBetaFilter(double a, double b, double dt):
	_a(a), _b(b), _dt(dt)
{
	_xk_1 = 0;
	_vk_1 = 0;
}

double AlphaBetaFilter::calculate(double in)
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
