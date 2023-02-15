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
    QString csvname1 = name_log_dir + "/preparedTable.csv";
    QFile csv1(csvname1);
    QString filename1 = name_log_dir + "/coefs.txt";
    QFile file1(filename1);
    QString csvname2 = name_log_dir + "/preparedTableUp.csv";
    QFile csv2(csvname2);
    double SOC {0.999};
    QVector<double> Ytemp;
    QVector<double> Itemp;
    QVector<double> Yp, Ip;

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



    //Find OCV function from SOC START
    MatrixXd ocv_OCV_m              (4, 1);
    MatrixXd ocv_Theta_m            (4, 1);
    MatrixXd ocv_SOC_m              (4, 4);
    ocv_OCV_m                       << 48.49, 49.46, 50.31, 51.2;
    ocv_SOC_m                       <<  pow(0.7, 3.0), pow(0.7, 2.0), 0.7, 1.0,
                                        pow(0.8, 3.0), pow(0.8, 2.0), 0.8, 1.0,
                                        pow(0.9, 3.0), pow(0.9, 2.0), 0.9, 1.0,
                                        1.0, 1.0, 1.0, 1.0;

    ocv_Theta_m = (ocv_SOC_m.transpose()*ocv_SOC_m).inverse()*ocv_SOC_m.transpose()*ocv_OCV_m;
    std::cout << "Find coefs for OCV(SOC):" << std::endl;
    std::cout << ocv_Theta_m << std::endl;
    //Find OCV function from SOC END

    //Find SOC function from OCV START
    MatrixXd soc_SOC_m              (4, 1);
    MatrixXd soc_Theta_m            (4, 1);
    MatrixXd soc_OCV_m              (4, 4);
    soc_SOC_m                       << 0.7, 0.8, 0.9, 1.0;
    soc_OCV_m                       <<  pow(48.49, 3.0), pow(48.49, 2.0), 48.49, 1.0,
                                        pow(49.46, 3.0), pow(49.46, 2.0), 49.46, 1.0,
                                        pow(50.31, 3.0), pow(50.31, 2.0), 50.31, 1.0,
                                        pow(51.2, 3.0), pow(51.2, 2.0), 51.2, 1.0;

    soc_Theta_m = (soc_OCV_m.transpose()*soc_OCV_m).inverse()*soc_OCV_m.transpose()*soc_SOC_m;
    std::cout << "Find coefs for SOC(OCV):" << std::endl;
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
    double SOC_result           {0.0};

    i = 0;
    while(i < Ytemp.size()){
        Y(i,0) = Ytemp.at(i);
        H_linear(i,0) = 1;
        H_linear(i,1) = Itemp.at(i);
        SOC += Itemp.at(i)/(14.985*3600.0);
        H_linear(i,2) = SOC;
// The identification part of Rp*(1-exp(-t/(Rp*Cp))) + R0
// Up = I*Rp*(1-exp(-t/(Rp*Cp))) +I*R0
        OCV_result              = ocv_Theta_m(0,0)*pow(H_linear(i,2),3.0);
        OCV_result              += ocv_Theta_m(1,0)*pow(H_linear(i,2),2.0);
        OCV_result              += ocv_Theta_m(2,0)*H_linear(i,2);
        OCV_result              += ocv_Theta_m(3,0)*1.0;

        Y_nl(i,0) = Ytemp.at(i) - OCV_result;
        H_nl(i,0) = prevUp;
        H_nl(i,1) = prevIp;

        prevUp = Y_nl(i,0);
        prevIp = Itemp.at(i);
        i++;
    }

    //Theta = (H^T * H)^-1 * H^T * Y
    Theta_linear = (H_linear.transpose()*H_linear).inverse()*H_linear.transpose()*Y;

    std::cout << "Find coefs for Unnewehr universal model:" << std::endl;
    std::cout << Theta_linear << std::endl;

    Theta_nl = (H_nl.transpose()*H_nl).inverse()*H_nl.transpose()*Y_nl;

    std::cout << "Find coefs for non-linear model of Up = I*Rp*(1-exp(-t/(Rp*Cp))) +I*R0:" << std::endl;
    std::cout << Theta_nl << std::endl;

    double K_0 = Theta_linear   (0,0);
    double R = Theta_linear     (1,0);
    double K_1 = Theta_linear   (2,0);
    int n                       {2}; //amount of states (SOC_k, I_k)
    int m                       {1}; //amount of measurements (Uout)
    MatrixXd P_k_1              (n,n);
    MatrixXd P_k                (n,n);

                                   //11      //12       //21       //22
    P_k_1                       << 7.524871, -0.455672, -0.455672, 2.162473;

    MatrixXd F_k                (n,n);
    F_k                         << 1.0, (1.0/(14.985*3600.0)), 0.0, 1.0;

    MatrixXd H_k                (m,n);
    H_k                         << K_1, R;

    MatrixXd Q_k                (n,n);
    Q_k                         << 0.1, 0.1, 0.1, 1.0;

    MatrixXd R_k                (m,m);

    R_k                         << 10.0;

    MatrixXd x_hat_k            (n,1), x_hat_k_1        (n,1);

    MatrixXd z_k                (m,1);

    MatrixXd K_k                (n,m);

    i = 0;
    SOC = 0.999;

    x_hat_k_1                   << SOC, 0.0;

    double Y_result             {0.0};
    while(i < Ytemp.size()){

        Y_linear_SUN_model      = K_0 * H_linear(i,0);           //1 linear
        Y_linear_SUN_model      += R * H_linear(i,1);            //2 linear
        Y_linear_SUN_model      += K_1 * H_linear(i,2);          //3 linear

        /*Kalman filter part START*/
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
        /*Kalman filter part END*/

        //Solution part
        Y_result                = K_0 + R*H_linear(i,1) + K_1*x_hat_k(0,0);

        OCV_result              = ocv_Theta_m(0,0)*pow(H_linear(i,2),3.0);
        OCV_result              += ocv_Theta_m(1,0)*pow(H_linear(i,2),2.0);
        OCV_result              += ocv_Theta_m(2,0)*H_linear(i,2);
        OCV_result              += ocv_Theta_m(3,0)*1.0;

        Up = H_nl(i,0)*Theta_nl(0,0) + H_nl(i,1)*Theta_nl(1,0);
        Y_non_linear = OCV_result + Up;

        SOC_result              = soc_Theta_m(0,0)*pow(Y(i,0),3.0);
        SOC_result              += soc_Theta_m(1,0)*pow(Y(i,0),2.0);
        SOC_result              += soc_Theta_m(2,0)*Y(i,0);
        SOC_result              += soc_Theta_m(3,0)*1.0;

        m_err_kf(i,0)           = Y(i,0) - Y_result;

        dataDB.clear();
        dataDB.append(QString::number(Itemp.at(i)));
        dataDB.append(QString::number(Y(i,0)));
        dataDB.append(QString::number(Y_linear_SUN_model));
        dataDB.append(QString::number(Y_result));
        dataDB.append(QString::number(OCV_result));
        dataDB.append(QString::number(SOC_result));
        dataDB.append(QString::number(Y_non_linear));
        dataDB.append(QString::number(0.0));

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
