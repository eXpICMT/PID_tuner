#include <QCoreApplication>

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <iostream>

#include "alphabetafilter.h"
#include "database.h"

//#define DEBUG_CMD_APP

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

	DB = database::getInstance();
	DB->connectToDataBase(name_log_dir);

	double alpha {0.85};
	double beta {0.005};
	double dt {1.0};

	AlphaBetaFilter filter(alpha, beta, dt);


#ifdef DEBUG_CMD_APP
	string strDatum {};
	int i {};
	double sum {};
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
			dataDB.append(datum_str);
			outcome = filter.calculate(datum.toDouble());
			datum_str = QString::number(outcome);
			dataDB.append(datum_str);

#ifdef DEBUG_CMD_APP
			qDebug() << "V(" << i << ") = " << datum;
			sum += datum.toDouble();
			qDebug() << "SUM(V(" << (i-i) << "..." << i << ")) = " << sum;
			i++;
#endif
		}
		file.close();
		DB->closeDataBase();
	}

	return a.exec();
}
