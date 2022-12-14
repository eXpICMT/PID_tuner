#include <QCoreApplication>

#include <QFile>
#include <QTextStream>
#include <QDebug>
//#include <iostream>
#include <math.h>
//#include "qtextstream.h"

//#include "alphabetafilter.h"
//#include "medianfilter.h"
#include "database.h"
#include <QStringList>

#define DEBUG_CMD_APP
#define DATABASE_ON

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

//using namespace std;
float Uoc_100_soc = 51.14;//SOC = 100%
float Uoc_90_soc = 49.89;//SOC = 90%
float deltaU_R0 = Uoc_100_soc-47.31;//SOD phase
float Ibat = 15.0;//SOD phase
float R0_100_sod = deltaU_R0/Ibat;//SOD phase
float R0_100_soc = {0.0};
float t1 = 10.0;//SOD phase
float t2 = 40.0;//Relax phase
float Up_t_1_1 = 47.31;
float Up_t_1_2 = 47.25;
float dUp_t_1 = abs(Up_t_1_2-Up_t_1_1);//SOD phase, where I = 15.0 A, tau = 10 s
float Up_t_2_1 = 50.06;//50.06
float Up_t_2_2 = 50.67;
float dUp_t_2_0 = abs(Up_t_2_2-Up_t_2_1);//(point,where t = 0) Relax phase, where I = 0.0, tau = 40 s
//Borders of dUp_t_2_0/dUp_t_1 should be beyond of the range
/* for example
50.06 { 50.08, 50.16, 50.21 } 50.67
	  \       manifold      /
*/

QByteArray datum;
QString datum_str;
float outcome;
int i {0}, r {0};

database* DB = nullptr;
QVariantList dataDB;

int main(int argc, char *argv[])
{

	QCoreApplication a(argc, argv);

	QString name_log_dir = QCoreApplication::applicationDirPath();
	QString filename1 = name_log_dir + "/t1_for_Up.txt";
	QFile file1(filename1);
	QTextStream fileText1{&file1};

	QString filename2 = name_log_dir + "/t2_for_Up.txt";
	QFile file2(filename2);
	QTextStream fileText2{&file2};

	QString csvname1 = name_log_dir + "/hppc_test_from_112122_rev.3.csv";
	QFile csv1(csvname1);

	QString filename3 = name_log_dir + "/coefs.txt";
	QFile file3(filename3);
	QTextStream fileText3{&file3};
	QVector<QString> buffer;


	float dU_p_i {0.0};
	float temp {0.0};
	float tau_p_mean {0.0};
	float R_p_mean {0.0};
	float C_p_mean {0.0};
	float dU {0.0};
	i = 0;
	outcome = 0.0;

	if(!file2.open(QIODevice::ReadOnly | QIODevice::ExistingOnly)){
		qWarning("The file 2 wasn't read");
	}else{
		while(!file2.atEnd()){
			datum.clear();
			datum = file2.readLine();
			datum = datum.trimmed();
			i++;
			dU_p_i = abs(Up_t_2_2-datum.toFloat());
			temp = dUp_t_2_0/dU_p_i;
			temp = log(temp);
			temp = 1/temp;
			temp = i*temp;
			outcome += temp;
			qDebug() << "Time = " << i << "s, tau_meas = " << temp;
			QString str = "Time = " + QString::number(i) + "s, tau_meas = " + QString::number(temp);
			buffer.push_back(str);
		}
		tau_p_mean = outcome/i;
		file2.close();
	}
	dU = dUp_t_2_0/i;
	outcome = 0.0;
	i = 0;
	DB = database::getInstance();
	DB->connectToDataBase(name_log_dir);

	if(!file1.open(QIODevice::ReadOnly | QIODevice::ExistingOnly)){
		qWarning("The file 1 wasn't read");
	}else{
		while(!file1.atEnd()){
			datum.clear();
			datum = file1.readLine();
			datum = datum.trimmed();
			i++;
			dU_p_i = abs(Up_t_1_2-datum.toFloat());
			temp = i/tau_p_mean;
			temp *= -1;
			temp = exp(temp);
			temp = 1 - temp;
			temp = Ibat*temp;
			temp = 1/temp;
			temp *= dU_p_i;
			outcome += temp;
			qDebug() << "Time = " << i << "s, Rp_meas = " << temp;
			QString str = "Time = " + QString::number(i) + "s, Rp_meas = " + QString::number(temp);
			buffer.push_back(str);
		}
		R_p_mean = outcome/i;
		qDebug() << "Corollary:";
		QString str = "Corollary:";
		buffer.push_back(str);
		qDebug() << "tau_mean = " << tau_p_mean;
		str = "tau_mean = " + QString::number(tau_p_mean);
		buffer.push_back(str);
		qDebug() << "R_mean = " << R_p_mean;
		str = "R_mean = " + QString::number(R_p_mean);
		buffer.push_back(str);
		C_p_mean = tau_p_mean/R_p_mean;
		qDebug() << "C_mean = " << C_p_mean;
		str = "C_mean = " + QString::number(C_p_mean);
		buffer.push_back(str);
		qDebug() << "R_0 = " << R0_100_soc;
		file1.close();
	}

	auto writeToFile = [&file3, buffer, &fileText3](){
		file3.resize(0);
		for(QString element : buffer)
			fileText3 << element << Qt::endl;
		file3.close();
	};

	if(file3.open(QIODevice::WriteOnly | QIODevice::ExistingOnly | QIODevice::Append | QIODevice::Text))
		writeToFile();
	else if(file3.open(QIODevice::WriteOnly | QIODevice::NewOnly | QIODevice::Append | QIODevice::Text))
		writeToFile();

	i = 0;
	temp = 1/tau_p_mean;
	temp *= -1;
	temp = exp(temp);

	auto constrain = [](float value, float max, float min){
		if(value > max)value = max;
		if(value < min)value = min;
	};

	if(!csv1.open(QIODevice::ReadOnly | QIODevice::ExistingOnly)){
		qWarning("The csv 1 wasn't read");
	}else{
		QTextStream in(&csv1);
		float Icsv {0.0}, Ucsv {0.0}, Iprev {0.0}, Uprev {0.0}, dI {0.0};// dU {0.0};
		float Icnt {0.0};
		float temp1 {0.0};
		float temp2 {0.0};
		bool flag {false};
		float R0 {0.0};
		while(!in.atEnd()){
			Iprev = Icsv;
			Uprev = Ucsv;
			QString line = in.readLine();
			i++;
			QStringList item = line.split(",");
			Icsv = item.at(0).toFloat();
			Ucsv = item.at(1).toFloat();
			Icnt += Icsv;
			float SOC = 1+(Icnt/(15*3600));
			constrain(SOC, 1, 0.9);
			float Uoc = xFromLineOfTwoPoints(SOC, 1, 0.9, Uoc_100_soc, Uoc_90_soc);
			// - dUp_t_2_0*temp
			temp = i /tau_p_mean;
			temp *= -1;
			temp = exp(temp);

			if((Icsv < -1.0f) || (Icsv > 1.0f)){
				//dU = dUp_t_1;
				dU = dUp_t_2_0;
				//if(Icsv < -1.0f)
				//	R0 = R0_100_sod;
				//if(Icsv > 1.0f)
					R0 = abs((Ucsv - Uoc_100_soc)/Icsv);
			}
			else
				dU = dUp_t_2_0;


			temp1 = dU*temp;

			float U_l = Uoc - temp1 + Icsv*R_p_mean*(1-temp) + Icsv*R0;
			dI = Icsv - Iprev;
			//dU = abs(Ucsv - Uprev);
			dataDB.clear();
			dataDB.append(QString::number(Ucsv));
			dataDB.append(QString::number(U_l));
			dataDB.append(QString::number(Icsv));
			if(!DB->inserIntoTable(TABLE1, dataDB))
			{
				//TODO
			}
			qDebug() << "Line[" << i << "]";
		}
	}
	csv1.close();
	DB->closeDataBase();

    return 0;
}
