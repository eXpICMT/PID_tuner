#include <QCoreApplication>

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <iostream>

#include "alphabetafilter.h"
#include "medianfilter.h"
#include "database.h"

#define DEBUG_CMD_APP
#define DATABASE_ON

using namespace std;



int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QString name_log_dir = QCoreApplication::applicationDirPath();
	QString filename = name_log_dir + "/test.txt";
	QFile file(filename);
	QTextStream fileText{&file};

	QByteArray datum;
	QString datum_str;
	double outcome;

	database* DB = nullptr;
	QVariantList dataDB;
#ifdef DATABASE_ON
    DB = database::getInstance();
    DB->connectToDataBase(name_log_dir);
#endif
	double alpha {0.85};
	double beta {0.005};
	double dt {1.0};

	AlphaBetaFilter<double,double> ab_filter(alpha, beta, dt);
	MedianFilter<double> median_filter_1st_order(11);
	MedianFilter<double> median_filter_2nd_order(11);
	MedianFilter<double> median_filter_3d_order(11);
	MedianFilter<double> median_filter_4th_order(11);
	MedianFilter<double> median_filter_5th_order(11);
	MedianFilter<double> median_filter_6th_order(11);

#ifdef DEBUG_CMD_APP
	string strDatum {};
	int i {}, flag {0};
	double du {}, u_prev {};
#endif

	if(!file.open(QIODevice::ReadOnly | QIODevice::ExistingOnly))
	{
		qWarning("The file wasn't read");
	}
	else
	{
		while(!file.atEnd())
		{
			datum.clear();
			datum = file.readLine();
			datum = datum.trimmed();

			dataDB.clear();
			datum_str = QString(datum);
			qDebug() << "datum(" << i << ") = " << datum_str;
			dataDB.append(datum_str);
			outcome = ab_filter.calculate(datum.toDouble());
			datum_str = QString::number(outcome);
			qDebug() << "Alpha-Beta(" << i << ") = " << datum_str;
			dataDB.append(datum_str);
			outcome = median_filter_1st_order.calculation(datum.toDouble());
			datum_str = QString::number(outcome);
			qDebug() << "Median_1st_order(" << i << ") = " << datum_str;
			dataDB.append(datum_str);
			outcome = median_filter_2nd_order.calculation(outcome);
			datum_str = QString::number(outcome);
			qDebug() << "Median_2nd_order(" << i << ") = " << datum_str;
			dataDB.append(datum_str);
			outcome = median_filter_3d_order.calculation(outcome);
			datum_str = QString::number(outcome);
			qDebug() << "Median_3d_order(" << i << ") = " << datum_str;
			dataDB.append(datum_str);
			if(flag)
			{
				du = outcome - u_prev;
				flag = 0;
			}
			else
			{
				u_prev = outcome;
				flag = 1;
			}
			if(du < 0.0)
			{
				outcome = u_prev;
			}
			datum_str = QString::number(outcome);
			qDebug() << "Track_du_filter(" << i << ") = " << datum_str;
			dataDB.append(datum_str);
			dataDB.append("0");
			dataDB.append("0");

#ifdef DATABASE_ON
            if(!DB->inserIntoTable(TABLE1, dataDB))
            {
                //TODO
            }
#endif
			i++;
		}
		file.close();
#ifdef DATABASE_ON
		DB->closeDataBase();
#endif
	}

	return a.exec();
}
