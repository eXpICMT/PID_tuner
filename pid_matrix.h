#ifndef PID_MATRIX_H
#define PID_MATRIX_H

template<typename A>
class PID_MATRIX_3x3
{
public:
    ///< e1*Kp + E1*Ki + de1*Kd = out1
    ///< e2*Kp + E2*Ki + de2*Kd = out2
    ///< e3*Kp + E3*Ki + de3*Kd = out3
    ///< PID_MATRIX_3x3(A e1, A E1, A de1, A out1, A e2, A E2, A de2, A out2, A e3, A E3, A de3, A out3);
    PID_MATRIX_3x3();
    void sendNewCoefs(A e1, A E1, A de1, A out1, A e2, A E2, A de2, A out2, A e3, A E3, A de3, A out3);
    A calculate_Kd();
    A calculate_Ki();
    A calculate_Kp();
private:
    A _e1, _e2, _e3;
    A _E1, _E2, _E3;
    A _de1, _de2, _de3;
    A _out1, _out2, _out3;
    A _Kd, _Ki, _Kp;
};

template<typename A>
PID_MATRIX_3x3<A>::PID_MATRIX_3x3()
{

}

template<typename A>
void PID_MATRIX_3x3<A>::sendNewCoefs(A e1, A E1, A de1, A out1, A e2, A E2, A de2, A out2, A e3, A E3, A de3, A out3)
{
    _e1 = e1;
    _E1 = E1;
    _de1 = de1;
    _out1 = out1;

    _e2 = e2;
    _E2 = E2;
    _de2 = de2;
    _out2 = out2;

    _e3 = e3;
    _E3 = E3;
    _de3 = de3;
    _out3 = out3;
}

template<typename A>
A PID_MATRIX_3x3<A>::calculate_Kd()
{
    A temp;
    temp =  (_de3*_e1 + _de1*_e3)/(_E3*_e1 - _E1*_e3);
    temp -= (_de2*_e1 + _de1*_e2)/(_E2*_e1 - _E1*_e2);
    _Kd =   (_out3*_e1 - _out1*_e3)/(_E3*_e1 - _E1*_e3);
    _Kd -=  (_out2*_e1 - _out1*_e1)/(_E2*_e1 - _E1*_e2);
    _Kd /=  temp;
    return _Kd;
}

template<typename A>
A PID_MATRIX_3x3<A>::calculate_Ki()
{
    _Ki =   (_out2*_e1 - _out1*_e2)/(_E2*_e1 - _E1*_e2);
    _Ki -=  _Kd*((_de2*_e1 + _de1*_e2)/(_E2*_e1 - _E1*_e2));
    return _Ki;
}

template<typename A>
A PID_MATRIX_3x3<A>::calculate_Kp()
{
    _Kp = (_out1 - _E1*_Ki +_de1*_Kd)/(_e1);
    return _Kp;
}

#endif // PID_MATRIX_H
