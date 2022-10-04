#include <QCoreApplication>

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <iostream>

#include "alphabetafilter.h"
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

	AlphaBetaFilter filter(alpha, beta, dt);


#ifdef DEBUG_CMD_APP
	string strDatum {};
	int i {};
    //double sum {};
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
			outcome = filter.calculate(datum.toDouble());
			datum_str = QString::number(outcome);
            qDebug() << "dataDB(" << i << ") = " << datum_str;
            dataDB.append(datum_str);
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
