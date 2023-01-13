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
    double SOC {1.0};
    QVector<double> Ytemp;
    QVector<double> Itemp;

    auto constrain = [](float value, float max, float min){
        if(value > max)value = max;
        if(value < min)value = min;
    };

    DB = database::getInstance();
    DB->connectToDataBase(name_log_dir);
    int i = 0;
    float Yest {0.0};

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

    MatrixXd Y(Ytemp.size(), 1);
    MatrixXd H(Ytemp.size(), 7), HTH(7,7), Htransposed(7,Ytemp.size()), HTHinversed(7,7);
    MatrixXd Theta(7, 1);
    MatrixXd m_err (Ytemp.size(), 1);

    i = 0;
    while(i < Ytemp.size()){
        Y(i,0) = Ytemp.at(i);
        H(i, 0) = 1;//for K0
        if(Itemp.at(i) > 0.0){
            H(i, 1) = Itemp.at(i);//for I+
            H(i, 2) = 0.0;//for I-
        }else{
            H(i, 1) = 0.0;
            H(i, 2) = Itemp.at(i);
        }
        SOC += Itemp.at(i)/(15.0*3600.0);
        if(SOC > 1.0)
            SOC = 1.0;
        H(i, 3) = 1.0/SOC;
        H(i, 4) = SOC;
        H(i, 5) = log(SOC);
        if((1-SOC) > 0.0)
            H(i, 6) = log(1-SOC);
        else
            H(i, 6) = 1.0;
        i++;
    }

    //Theta = (H^T * H)^-1 * H^T * Y

    Htransposed = H.transpose();
    HTH = Htransposed*H;
    HTHinversed = HTH.inverse();
    Theta = HTHinversed*Htransposed;
    Theta *= Y;

    std::cout << "Theta:" << std::endl;
    std::cout << Theta << std::endl;

    i = 0;

    while(i < Ytemp.size()){
        Yest = Theta(0,0) * H(i,0);          //1
        Yest += Theta(1,0) * H(i,1);         //2
        Yest += Theta(2,0) * H(i,2);         //3
        Yest += Theta(3,0) * H(i,3);         //4
        Yest += Theta(4,0) * H(i,4);         //5
        Yest += Theta(5,0) * H(i,5);         //6
        Yest += Theta(6,0) * H(i,6);         //7
        m_err(i, 0) = Y(i,0) - Yest;
        dataDB.clear();
        dataDB.append(QString::number(Y(i,0)));
        dataDB.append(QString::number(Yest));
        dataDB.append(QString::number(Itemp.at(i)));
        if(!DB->inserIntoTable(TABLE1, dataDB))
        {
            //TODO
        }
        i++;
    }
    m_err = m_err.transpose()*m_err;
    m_err /= Ytemp.size();
    std::cout << "Mean squared error = " << m_err;

/* Result
    Theta:
       51.1968
      0.361198
      0.341589
       15.1891
      -15.4414
       37.8947
    -0.0317025
*/

    csv1.close();
    DB->closeDataBase();

    return 0;

}
