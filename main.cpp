#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <math.h>
#include "database.h"
#include <QStringList>
#include "Eigen"
#include <iostream>
#include <QVector>

/* Description
 * The model is combination of:
 * Shepherd model:              yk = E0 − R*ik − Ki/zk
 * Unnewehr universal model:    yk = E0 − R*ik − Ki*zk
 * Nernst model:                yk = E0 − R*ik + K2*ln(zk)+K3*ln(1−zk)
 * where
 * ik - measured current [A]
 * zk - measured SOC
*/

/*
    Battery Model Parameters
    Open Circuit Voltage
      Uocv_100%       = 51.2
      Uocv_90%        = 50.31
      Uocv_80%        = 49.46
      Uocv_70%        = 48.49
      State of Charge
      SOC_100%        = 15
      SOC_90%         = 13.5
      SOC_80%         = 12
      SOC_70%         = 10.5
      Internal Resistance
      Rin-_100%       = (51.2 - 47.71)/15.1
      Rin+_100%       = (59.71 - 50.67)/7.35
      Rin-_90%        = (50.31 - 47.1)/15.1
      Rin+_90%        = (58.78 - 49.98)/15.86
      Rin-_80%        = (49.46 - 46.6)/15.09
      Rin+_80%        = (54.9 - 49.14)/15.85
      Rin-_70%        = (48.49 - 45.71)/15.1
      Rin+_70%        = (53.83 - 48.38)/15.85
      float SOC = 1+(Icnt/(15*3600));
*/

using Eigen::MatrixXf;
using Eigen::MatrixXd;

int xFromLineOfTwoPoints(float y, float y1, float y2, int x1, int x2);
    //		  0.9soc	1soc	  Uoc0.9  Uoc1
int xFromLineOfTwoPoints(float y, float y1, float y2, int x1, int x2)
{
    int x = 0;
    x = y-y1;
    x *= (x2-x1);
    x /= (y2-y1);
    x += x1;
    return x;
}

int main(int argc, char *argv[]){
    database* DB = nullptr;
    QVariantList dataDB;
    QCoreApplication a(argc, argv);
    QString name_log_dir = QCoreApplication::applicationDirPath();
    QString csvname1 = name_log_dir + "/preparedTable_lifepo4.csv";
    QFile csv1(csvname1);
    QString filename1 = name_log_dir + "/coefs.txt";
    QFile file1(filename1);
    QString csvname2 = name_log_dir + "/preparedTableUp.csv";
    QFile csv2(csvname2);

    QString csvname3 = name_log_dir + "/ekfpid_vs_pid_3.csv";
    QFile csv3(csvname3);
    double SOC {0.999};
    QVector<double> Ytemp;
    QVector<double> Itemp;
    QVector<double> Yp, Ip;

    QVector<double> Ypid, Yekf_pid;
    QVector<double> Ipid, Iekf_pid;
    QVector<double> I_src_pid, I_src_ekf_pid;

    auto constrain = [](float value, float max, float min){
        if(value > max)value = max;
        if(value < min)value = min;
    };

    DB = database::getInstance();
    DB->connectToDataBase(name_log_dir);
    int i = 0;
    float Y_linear_SUN_model {0.0};
    float Y_non_linear {0.0};
    float Up = 0.0;
    float prevUp = 0.0;
    float prevIp = 0.0;
    float prevSOC = 0.0;

    if(!csv1.open(QIODevice::ReadOnly | QIODevice::ExistingOnly)){
        qWarning("The csv 1 wasn't read");
    }else{
        QTextStream in1(&csv1);
        while(!in1.atEnd()){

            double Icsv {0.0}, Ucsv {0.0};
            QString line = in1.readLine();
            QStringList item = line.split(",");
            Icsv = item.at(0).toDouble();
            Itemp.push_back(Icsv);
            Ucsv = item.at(1).toDouble();
            Ytemp.push_back(Ucsv);
        }
        csv1.close();
    }

    if(!csv2.open(QIODevice::ReadOnly | QIODevice::ExistingOnly)){
        qWarning("The csv 2 wasn't read");
    }else{
        QTextStream in2(&csv2);
        while(!in2.atEnd()){

            double Icsv {0.0}, Ucsv {0.0};
            QString line = in2.readLine();
            QStringList item = line.split(",");
            Icsv = item.at(0).toDouble();
            Ip.push_back(Icsv);
            Ucsv = item.at(1).toDouble();
            Yp.push_back(Ucsv);
        }
        csv2.close();
    }

    if(!csv3.open(QIODevice::ReadOnly | QIODevice::ExistingOnly)){
        qWarning("The csv 3 wasn't read");
    }else{
        QTextStream in3(&csv3);
        while(!in3.atEnd()){
            double Icsv {0.0}, Ucsv {0.0};
            QString line = in3.readLine();
            QStringList item = line.split(",");
            Icsv = item.at(0).toDouble();
            Ip.push_back(Icsv);
            Ucsv = item.at(1).toDouble();
            Yp.push_back(Ucsv);

            Ipid.push_back(item.at(0).toDouble());
            Ypid.push_back(item.at(1).toDouble());
            I_src_pid.push_back(item.at(2).toDouble());

            Iekf_pid.push_back(item.at(3).toDouble());
            Yekf_pid.push_back(item.at(4).toDouble());
            I_src_ekf_pid.push_back(item.at(5).toDouble());
        }
        csv3.close();
    }


    //Find OCV function from SOC START
    MatrixXd ocv_OCV_m              (3, 1);
    MatrixXd ocv_Theta_m            (3, 1);
    MatrixXd ocv_SOC_m              (3, 3);
    ocv_OCV_m                       << 22.01, 22.39, 22.79;//48.49, 49.46, 50.31, 51.2;
    ocv_SOC_m                       << pow(0.8, 2.0), 0.8, 1.0,
        pow(0.9, 2.0), 0.9, 1.0,
        1.0, 1.0, 1.0;

    ocv_Theta_m = (ocv_SOC_m.transpose()*ocv_SOC_m).inverse()*ocv_SOC_m.transpose()*ocv_OCV_m;
    std::cout << "Find coefs for OCV(SOC) 4:" << std::endl;
    std::cout << ocv_Theta_m << std::endl;
    //Find OCV function from SOC END

    //Find OCV function from SOC START
    MatrixXd ocv1_OCV_m              (3, 1);
    MatrixXd ocv1_Theta_m            (4, 1);
    MatrixXd ocv1_SOC_m              (3, 4);
    ocv1_OCV_m                       << 22.01, 22.39, 22.79;
    ocv1_SOC_m                       << pow(0.8, 3.0), pow(0.8, 2.0), 0.8, 1.0,
        pow(0.9, 3.0), pow(0.9, 2.0), 0.9, 1.0,
        1.0, 1.0, 1.0, 1.0;

    ocv1_Theta_m = (ocv1_SOC_m.transpose()*ocv1_SOC_m).inverse()*ocv1_SOC_m.transpose()*ocv1_OCV_m;
    std::cout << "Find coefs for OCV(SOC) 5:" << std::endl;
    std::cout << ocv1_Theta_m << std::endl;
    //Find OCV function from SOC END

    //Find SOC function from OCV START
    MatrixXd soc_SOC_m              (3, 1);
    MatrixXd soc_Theta_m            (3, 1);
    MatrixXd soc_OCV_m              (3, 3);
    soc_SOC_m                       << 0.8, 0.9, 1.0;
    soc_OCV_m                       << pow(22.01, 2.0), 22.01, 1.0,
        pow(22.39, 2.0), 22.39, 1.0,
        pow(22.79, 2.0), 22.79, 1.0;

    soc_Theta_m = (soc_OCV_m.transpose()*soc_OCV_m).inverse()*soc_OCV_m.transpose()*soc_SOC_m;
    std::cout << "Find coefs for SOC(OCV) 4:" << std::endl;
    std::cout << soc_Theta_m << std::endl;
    //Find SOC function from OCV END

    //Find Unnewehr universal model
    MatrixXd Y                  (Ytemp.size(), 1);
    MatrixXd H_linear           (Ytemp.size(), 3);
    MatrixXd Theta_linear       (3, 1);
    MatrixXd m_err_linear       (Ytemp.size(), 1);
    MatrixXd m_err_kf           (Ytemp.size(), 1);

    MatrixXd Y_nl                (Ytemp.size(), 1);//Y non linear
    MatrixXd H_nl                (Ytemp.size(), 2);//2
    MatrixXd Theta_nl            (2,1);//(2,1)

    double OCV_result           {0.0};
    double OCV1_result          {0.0};
    double dOCV_result          {0.0};
    double SOC_result           {0.0};

    i = 0;
    prevSOC = SOC;
    while(i < Ytemp.size()){
        Y(i,0) = Ytemp.at(i);
        H_linear(i,0) = 1;
        H_linear(i,1) = Itemp.at(i);
        SOC += Itemp.at(i)/(4.95*3600.0);
        H_linear(i,2) = SOC;
        // The identification part of Rp*(1-exp(-t/(Rp*Cp))) + R0
        // Up = I*Rp*(1-exp(-t/(Rp*Cp))) +I*R0
        OCV_result              = ocv_Theta_m(0,0)*pow(prevSOC,2.0);
        OCV_result              += ocv_Theta_m(1,0)*prevSOC;
        OCV_result              += ocv_Theta_m(2,0)*1.0;

        Y_nl(i,0) = Ytemp.at(i) - OCV_result;
        H_nl(i,0) = prevSOC;
        H_nl(i,1) = prevIp;

        //prevUp = Y_nl(i,0);
        prevIp = Itemp.at(i);
        prevSOC = SOC;
        i++;
    }

    //Theta = (H^T * H)^-1 * H^T * Y
    Theta_linear = (H_linear.transpose()*H_linear).inverse()*H_linear.transpose()*Y;

    std::cout << "Find coefs for Unnewehr universal model:" << std::endl;
    std::cout << Theta_linear << std::endl;

    Theta_nl = (H_nl.transpose()*H_nl).inverse()*H_nl.transpose()*Y_nl;

    std::cout << "Find coefs for non-linear model:" << std::endl;
    std::cout << Theta_nl << std::endl;

    /*Standard Kalman filter part initialization START*/
    double K_0 = Theta_linear   (0,0);
    double R = Theta_linear     (1,0);
    double K_1 = Theta_linear   (2,0);
    int n                       {2}; //amount of states (SOC_k, I_k)
    int m                       {1}; //amount of measurements (Uout)
    MatrixXd P_k_1              (n,n);
    MatrixXd P_k                (n,n);
    float q1 {0.0}, sigma2 {0.0};

    //11      //12       //21       //22
    P_k_1                       << 2.141613, -0.081825, -0.081825, 0.209730;

    MatrixXd F_k                (n,n);
    F_k                         << 1.0, (1.0/(4.95*3600.0)), 0.0, 1.0;

    MatrixXd H_k                (m,n);
    H_k                         << K_1, R;

    q1 = 1.0/(4.95*3600.0);
    sigma2 = 0.05;

    MatrixXd Q_k                (n,n);
    Q_k                         << pow(q1,2.0)*sigma2, q1*sigma2, q1*sigma2, sigma2;

    MatrixXd R_k                (m,m);
    //0.100955035158673
    R_k                         << 0.1;

    MatrixXd x_hat_k            (n,1), x_hat_k_1        (n,1);

    MatrixXd z_k                (m,1);

    MatrixXd K_k                (n,m);

    /*Standard Kalman filter part initialization END*/

    /*Extended Kalman filter part initialization START*/
    double C1 = Theta_nl        (0,0);// for SOC_k_1
    double C2 = Theta_nl        (1,0);// for I_k_1
    //double OCV0                 {51.17};
    int x                       {2}; //amount of states (SOC_k, Up_k)
    int y                       {1}; //amount of measurements (Uout)
    MatrixXd P_ekf_k_1          (x,x);
    MatrixXd P_ekf_k            (x,x);

    //11      //12       //21       //22
    P_ekf_k_1                   << 2.141613, -0.081825, -0.081825, 0.209730;//<< 7.524871, -0.455672, -0.455672, 2.162473;

    MatrixXd A_k                (x,x);
    A_k                         = F_k;

    MatrixXd C_k                (y,x);

    MatrixXd H_ekf_k            (y,x);

    //Q_k and R_k from Standard KF

    MatrixXd x_ekf_hat_k            (x,1), x_ekf_hat_k_1        (x,1);

    MatrixXd z_ekf_k                (y,1);

    MatrixXd K_ekf_k                (x,y);

    float err_nl                    {0.0};

    /*Extended Kalman filter part initialization END*/

    i = 0;
    SOC = 0.999;

    x_hat_k_1                   << SOC, 0.0;
    x_ekf_hat_k_1               << SOC, 0.0;//SOC_0, I_0

    double Y_result             {0.0};
    double Y_ekf_result         {0.0};
    double I_ekf_result         {0.0};
    while(i < Ytemp.size()){

        Y_linear_SUN_model      = K_0 * H_linear(i,0);           //1 linear
        Y_linear_SUN_model      += R * H_linear(i,1);            //2 linear
        Y_linear_SUN_model      += K_1 * H_linear(i,2);          //3 linear

        /*Standard Kalman filter solution part START*/
        m_err_linear(i, 0)      = Y(i,0) - Y_linear_SUN_model;
        z_k(0,0)                = Y(i,0) - K_0;

        x_hat_k_1(1,0)          = H_linear(i,1);            //I_k_1

        //time update step
        x_hat_k                 = F_k*x_hat_k_1;

        P_k                     = F_k*P_k_1*F_k.transpose() + Q_k;

        //measurement update
        K_k                     = P_k*H_k.transpose()*((H_k*P_k*H_k.transpose() + R_k).inverse());

        x_hat_k                 = x_hat_k + K_k*(z_k - H_k*x_hat_k);

        P_k                     = P_k - K_k*H_k*P_k;

        //initialization to the next cycle
        x_hat_k_1(0,0)          = x_hat_k(0,0);                 //SOC_k_1

        P_k_1                   = P_k;
        /*Standard Kalman filter solution part END*/

        /*Standard Kalman filter solution part START*/

        //time update step

        z_ekf_k(0,0)            = Y(i,0);

        x_ekf_hat_k_1(1,0)      = H_nl(i,1);

        x_ekf_hat_k             = A_k*x_ekf_hat_k_1;

        P_ekf_k                 = A_k*P_ekf_k_1*A_k.transpose() + Q_k;

        //measurement update

        dOCV_result             = 2.0*ocv_Theta_m(0,0)*x_ekf_hat_k(0,0);
        dOCV_result             += ocv_Theta_m(1,0);

        C_k                     << dOCV_result, C1;

        K_ekf_k                 = P_ekf_k*C_k.transpose()*((C_k*P_ekf_k*C_k.transpose() + R_k).inverse());

        OCV_result              = ocv_Theta_m(0,0)*pow(x_ekf_hat_k(0,0),1.0);
        OCV_result              += ocv_Theta_m(1,0);
        OCV_result              += ocv_Theta_m(2,0)/x_ekf_hat_k(0,0);

        Up = C1 + OCV_result;
        H_ekf_k                 << Up , C2;

        x_ekf_hat_k             = x_ekf_hat_k + K_ekf_k*(z_ekf_k - H_ekf_k*x_ekf_hat_k);

        P_ekf_k                 = P_ekf_k - K_ekf_k*C_k*P_ekf_k;

        //initialization to the next cycle

        x_ekf_hat_k_1(0,0)      = x_ekf_hat_k(0,0);

        P_ekf_k_1               = P_ekf_k;

        /*Standard Kalman filter solution part END*/

        //Solution part
        Y_result                = K_0 + R*H_linear(i,1) + K_1*x_hat_k(0,0);

        SOC_result              = soc_Theta_m(0,0)*pow(Y(i,0),2.0);
        SOC_result              += soc_Theta_m(1,0)*Y(i,0);
        SOC_result              += soc_Theta_m(2,0)*1.0;

        OCV_result              = ocv_Theta_m(0,0)*pow(H_nl(i,0),2.0);
        OCV_result              += ocv_Theta_m(1,0)*H_nl(i,0);
        OCV_result              += ocv_Theta_m(2,0)*1.0;

        Y_non_linear = OCV_result + C1*H_nl(i,0) + C2*H_nl(i,1);

        OCV_result              = ocv_Theta_m(0,0)*pow(x_ekf_hat_k(0,0),2.0);
        OCV_result              += ocv_Theta_m(1,0)*x_ekf_hat_k(0,0);
        OCV_result              += ocv_Theta_m(2,0)*1.0;

        Y_ekf_result            = OCV_result + C1*x_ekf_hat_k(0,0) + C2*x_ekf_hat_k(1,0);
        I_ekf_result            = x_ekf_hat_k(1,0);

        m_err_kf(i,0)           = Y(i,0) - Y_result;

        err_nl = abs(Y_non_linear - Y(i,0));

        OCV1_result              = ocv1_Theta_m(0,0)*pow(H_nl(i,0),3.0);
        OCV1_result              += ocv1_Theta_m(1,0)*pow(H_nl(i,0),2.0);
        OCV1_result              += ocv1_Theta_m(2,0)*H_nl(i,0);
        OCV1_result              += ocv1_Theta_m(3,0)*1.0;

        dataDB.clear();
        dataDB.append(QString::number(Itemp.at(i)));
        dataDB.append(QString::number(Y(i,0)));
        dataDB.append(QString::number(Y_linear_SUN_model));
        dataDB.append(QString::number(Y_result));
        dataDB.append(QString::number(OCV_result));
        dataDB.append(QString::number(SOC_result));
        dataDB.append(QString::number(Y_non_linear));
        dataDB.append(QString::number(Y_ekf_result));
        dataDB.append(QString::number(err_nl));
        dataDB.append(QString::number(I_ekf_result));

        if(i < Ipid.size()){
            dataDB.append(QString::number(Ipid.at(i)));
            dataDB.append(QString::number(Ypid.at(i)));
            dataDB.append(QString::number(I_src_pid.at(i)));

            dataDB.append(QString::number(Iekf_pid.at(i)));
            dataDB.append(QString::number(Yekf_pid.at(i)));
            dataDB.append(QString::number(I_src_ekf_pid.at(i)));
        }
        else{
            dataDB.append(QString::number(0.0));
            dataDB.append(QString::number(0.0));
            dataDB.append(QString::number(0.0));

            dataDB.append(QString::number(0.0));
            dataDB.append(QString::number(0.0));
            dataDB.append(QString::number(0.0));
        }

        if(!DB->inserIntoTable(TABLE1, dataDB))
        {
            //TODO
        }
        i++;
    }
    m_err_linear = m_err_linear.transpose()*m_err_linear;
    m_err_linear /= Ytemp.size();

    m_err_kf = m_err_kf.transpose()*m_err_kf;
    m_err_kf /= Ytemp.size();
    std::cout << "Mean squared error (model) = " << m_err_linear << std::endl;
    std::cout << "Mean squared error (KF) = " << m_err_kf << std::endl;

    /* Result
    Theta:
 42.7595    //K0
0.361157    //R+
 0.34333    //R-
 8.35822    //K1
42.759
0.344552
8.36165
*/

    /* Kalman filter part
x_k - the actual SOC
z_k - the actual terminal voltage
x_tilda_k - the approxiate SOC
z_tilda_k - the approxiate terminal voltage
x_hat_k - an a_posteriori estimate of SOC
w_k - the process noise
v_k - the measurement noise
A - the Jacobian matrix of partial derivatives of f with respect to x
1
x_hat_tilda_k   = f(x_hat_tilda_k_1, u_k_1, 0);
SOC_hat_tilda_k = SOC_hat_k_1 + (I_k_1*delta_t/Cn);
*/

    DB->closeDataBase();

    return 0;

}
