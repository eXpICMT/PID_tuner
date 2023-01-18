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
    double SOC {0.999};
    QVector<double> Ytemp;
    QVector<double> Itemp;

    auto constrain = [](float value, float max, float min){
        if(value > max)value = max;
        if(value < min)value = min;
    };

    DB = database::getInstance();
    DB->connectToDataBase(name_log_dir);
    int i = 0;
    float Y_linear_SUN_model {0.0};

    if(!csv1.open(QIODevice::ReadOnly | QIODevice::ExistingOnly)){
        qWarning("The csv 1 wasn't read");
    }else{
        QTextStream in(&csv1);
        while(!in.atEnd()){

            double Icsv {0.0}, Ucsv {0.0};
            QString line = in.readLine();
            QStringList item = line.split(",");
            Icsv = item.at(0).toDouble();
            Itemp.push_back(Icsv);
            Ucsv = item.at(1).toDouble();
            Ytemp.push_back(Ucsv);
            i++;
        }
    }

    MatrixXd Y                  (Ytemp.size(), 1);
    MatrixXd H_linear           (Ytemp.size(), 3);
    MatrixXd HTH_linear         (3,3);
    MatrixXd Htransposed_linear (3,Ytemp.size());
    MatrixXd HTHinversed_linear (3,3);
    MatrixXd Theta_linear       (3, 1);
    MatrixXd m_err_linear       (Ytemp.size(), 1);
    MatrixXd m_err_kf_R_0_005   (Ytemp.size(), 1);
    MatrixXd m_err_kf_R_0_05    (Ytemp.size(), 1);
    MatrixXd m_err_kf_R_0_5     (Ytemp.size(), 1);
    MatrixXd m_err_kf_R_5       (Ytemp.size(), 1);

    i = 0;
    while(i < Ytemp.size()){
        Y(i,0) = Ytemp.at(i);
        H_linear(i,0) = 1;
        H_linear(i,1) = Itemp.at(i);
        SOC += Itemp.at(i)/(14.985*3600.0);
        H_linear(i,2) = SOC;
        i++;
    }

    //Theta = (H^T * H)^-1 * H^T * Y

    Htransposed_linear = H_linear.transpose();
    HTH_linear = Htransposed_linear*H_linear;
    HTHinversed_linear = HTH_linear.inverse();
    Theta_linear = HTHinversed_linear*Htransposed_linear;
    Theta_linear *= Y;

    std::cout << "Theta_linear:" << std::endl;
    std::cout << Theta_linear << std::endl;

    double K_0 = Theta_linear   (0,0);
    double R = Theta_linear     (1,0);
    double K_1 = Theta_linear   (2,0);
    int n                       {2}; //amount of states (SOC_k, I_k)
    int m                       {1}; //amount of measurements (Uout)
    MatrixXd P_k_1_R_0_005      (n,n);
    MatrixXd P_k_1_R_0_05       (n,n);
    MatrixXd P_k_1_R_0_5        (n,n);
    MatrixXd P_k_1_R_5          (n,n);
    MatrixXd P_k_R_0_005        (n,n);
    MatrixXd P_k_R_0_05         (n,n);
    MatrixXd P_k_R_0_5          (n,n);
    MatrixXd P_k_R_5            (n,n);

                                   //11      //12       //21       //22
    P_k_1_R_0_005               << 7.524871, -0.455672, -0.455672, 2.162473;
    P_k_1_R_0_05                << 7.524871, -0.455672, -0.455672, 2.162473;
    P_k_1_R_0_5                 << 7.524871, -0.455672, -0.455672, 2.162473;
    P_k_1_R_5                   << 7.524871, -0.455672, -0.455672, 2.162473;

    MatrixXd F_k                (n,n);
    F_k                         << 1.0, (1.0/(14.985*3600.0)), 0.0, 1.0;

    MatrixXd H_k                (m,n);
    H_k                         << K_1, R;

    MatrixXd Q_k                (n,n);
    Q_k                         << 0.1, 0.1, 0.1, 0.1;

    MatrixXd R_k_0_005          (m,m);
    MatrixXd R_k_0_05           (m,m);
    MatrixXd R_k_0_5            (m,m);
    MatrixXd R_k_5              (m,m);

    R_k_0_005                   << 0.005;
    R_k_0_05                    << 0.05;
    R_k_0_5                     << 0.5;
    R_k_5                       << 5.0;

    MatrixXd x_hat_k_R_0_005    (n,1), x_hat_k_1_R_0_005    (n,1);
    MatrixXd x_hat_k_R_0_05     (n,1), x_hat_k_1_R_0_05     (n,1);
    MatrixXd x_hat_k_R_0_5      (n,1), x_hat_k_1_R_0_5      (n,1);
    MatrixXd x_hat_k_R_5        (n,1), x_hat_k_1_R_5        (n,1);

    MatrixXd z_k                (m,1);

    MatrixXd K_k_R_0_005        (n,m);
    MatrixXd K_k_R_0_05         (n,m);
    MatrixXd K_k_R_0_5          (n,m);
    MatrixXd K_k_R_5            (n,m);

    i = 0;
    SOC = 0.999;

    x_hat_k_1_R_0_005           << SOC, 0.0;
    x_hat_k_1_R_0_05            << SOC, 0.0;
    x_hat_k_1_R_0_5             << SOC, 0.0;
    x_hat_k_1_R_5               << SOC, 0.0;

    double Y_result_R_0_005     {0.0};
    double Y_result_R_0_05      {0.0};
    double Y_result_R_0_5       {0.0};
    double Y_result_R_5         {0.0};

    while(i < Ytemp.size()){

        Y_linear_SUN_model      = K_0 * H_linear(i,0);           //1 linear
        Y_linear_SUN_model      += R * H_linear(i,1);            //2 linear
        Y_linear_SUN_model      += K_1 * H_linear(i,2);          //3 linear

        /*Kalman filter part*/
        m_err_linear(i, 0)      = Y(i,0) - Y_linear_SUN_model;
        z_k(0,0)                = Y(i,0) - K_0;

        x_hat_k_1_R_0_005(1,0)  = H_linear(i,1);            //I_k_1
        x_hat_k_1_R_0_05(1,0)   = H_linear(i,1);            //I_k_1
        x_hat_k_1_R_0_5(1,0)    = H_linear(i,1);            //I_k_1
        x_hat_k_1_R_5(1,0)      = H_linear(i,1);            //I_k_1

        //time update step
        x_hat_k_R_0_005         = F_k*x_hat_k_1_R_0_005;
        x_hat_k_R_0_05          = F_k*x_hat_k_1_R_0_05;
        x_hat_k_R_0_5           = F_k*x_hat_k_1_R_0_5;
        x_hat_k_R_5             = F_k*x_hat_k_1_R_5;

        P_k_R_0_005             = F_k*P_k_1_R_0_005*F_k.transpose() + Q_k;
        P_k_R_0_05              = F_k*P_k_1_R_0_05*F_k.transpose() + Q_k;
        P_k_R_0_5               = F_k*P_k_1_R_0_5*F_k.transpose() + Q_k;
        P_k_R_5                 = F_k*P_k_1_R_5*F_k.transpose() + Q_k;

        //measurement update
        K_k_R_0_005             = P_k_R_0_005*H_k.transpose()*((H_k*P_k_R_0_005*H_k.transpose() + R_k_0_005).inverse());
        K_k_R_0_05              = P_k_R_0_05*H_k.transpose()*((H_k*P_k_R_0_05*H_k.transpose() + R_k_0_05).inverse());
        K_k_R_0_5               = P_k_R_0_5*H_k.transpose()*((H_k*P_k_R_0_5*H_k.transpose() + R_k_0_5).inverse());
        K_k_R_5                 = P_k_R_5*H_k.transpose()*((H_k*P_k_R_5*H_k.transpose() + R_k_5).inverse());

        x_hat_k_R_0_005         = x_hat_k_R_0_005 + K_k_R_0_005*(z_k - H_k*x_hat_k_R_0_005);
        x_hat_k_R_0_05          = x_hat_k_R_0_05 + K_k_R_0_05*(z_k - H_k*x_hat_k_R_0_05);
        x_hat_k_R_0_5           = x_hat_k_R_0_5 + K_k_R_0_5*(z_k - H_k*x_hat_k_R_0_5);
        x_hat_k_R_5             = x_hat_k_R_5 + K_k_R_5*(z_k - H_k*x_hat_k_R_5);

        P_k_R_0_005             = P_k_R_0_005 - K_k_R_0_005*H_k*P_k_R_0_005;
        P_k_R_0_05              = P_k_R_0_05 - K_k_R_0_05*H_k*P_k_R_0_05;
        P_k_R_0_5               = P_k_R_0_5 - K_k_R_0_5*H_k*P_k_R_0_5;
        P_k_R_5                 = P_k_R_5 - K_k_R_5*H_k*P_k_R_5;

        //initialization to the next cycle
        x_hat_k_1_R_0_005(0,0)  = x_hat_k_R_0_005(0,0);             //SOC_k_1
        x_hat_k_1_R_0_05(0,0)   = x_hat_k_R_0_05(0,0);              //SOC_k_1
        x_hat_k_1_R_0_5(0,0)    = x_hat_k_R_0_5(0,0);               //SOC_k_1
        x_hat_k_1_R_5(0,0)      = x_hat_k_R_5(0,0);                 //SOC_k_1

        P_k_1_R_0_005           = P_k_R_0_005;
        P_k_1_R_0_05            = P_k_R_0_05;
        P_k_1_R_0_5             = P_k_R_0_5;
        P_k_1_R_5               = P_k_R_5;
        /*Kalman filter part*/

        //Solution part
        Y_result_R_0_005        = K_0 + R*H_linear(i,1) + K_1*x_hat_k_R_0_005(0,0);
        Y_result_R_0_05         = K_0 + R*H_linear(i,1) + K_1*x_hat_k_R_0_05(0,0);
        Y_result_R_0_5          = K_0 + R*H_linear(i,1) + K_1*x_hat_k_R_0_5(0,0);
        Y_result_R_5            = K_0 + R*H_linear(i,1) + K_1*x_hat_k_R_5(0,0);

        m_err_kf_R_0_005(i,0)   = Y(i,0) - Y_result_R_0_005;
        m_err_kf_R_0_05(i,0)    = Y(i,0) - Y_result_R_0_05;
        m_err_kf_R_0_5(i,0)     = Y(i,0) - Y_result_R_0_5;
        m_err_kf_R_5(i,0)       = Y(i,0) - Y_result_R_5;



        dataDB.clear();
        dataDB.append(QString::number(Itemp.at(i)));
        dataDB.append(QString::number(Y(i,0)));
        dataDB.append(QString::number(Y_linear_SUN_model));
        dataDB.append(QString::number(Y_result_R_0_005));
        dataDB.append(QString::number(Y_result_R_0_05));
        dataDB.append(QString::number(Y_result_R_0_5));
        dataDB.append(QString::number(Y_result_R_5));

        if(!DB->inserIntoTable(TABLE1, dataDB))
        {
            //TODO
        }
        i++;
    }
    m_err_linear = m_err_linear.transpose()*m_err_linear;
    m_err_linear /= Ytemp.size();

    m_err_kf_R_0_005 = m_err_kf_R_0_005.transpose()*m_err_kf_R_0_005;
    m_err_kf_R_0_005 /= Ytemp.size();
    std::cout << "Mean squared error (model) = " << m_err_linear << std::endl;
    std::cout << "Mean squared error (kf_R_0_005) = " << m_err_kf_R_0_005 << std::endl;

    m_err_kf_R_0_05 = m_err_kf_R_0_05.transpose()*m_err_kf_R_0_05;
    m_err_kf_R_0_05 /= Ytemp.size();
    std::cout << "Mean squared error (model) = " << m_err_linear << std::endl;
    std::cout << "Mean squared error (kf_R_0_05) = " << m_err_kf_R_0_05 << std::endl;

    m_err_kf_R_0_5 = m_err_kf_R_0_5.transpose()*m_err_kf_R_0_5;
    m_err_kf_R_0_5 /= Ytemp.size();
    std::cout << "Mean squared error (model) = " << m_err_linear << std::endl;
    std::cout << "Mean squared error (kf_R_0_5) = " << m_err_kf_R_0_5 << std::endl;

    m_err_kf_R_5 = m_err_kf_R_5.transpose()*m_err_kf_R_5;
    m_err_kf_R_5 /= Ytemp.size();
    std::cout << "Mean squared error (model) = " << m_err_linear << std::endl;
    std::cout << "Mean squared error (kf_R_5) = " << m_err_kf_R_5 << std::endl;

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
    csv1.close();
    DB->closeDataBase();

    return 0;

}
