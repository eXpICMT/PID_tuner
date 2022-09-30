#include <QCoreApplication>

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <iostream>

#define DEBUG_CMD_APP

using namespace std;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QString name_log_dir = QCoreApplication::applicationDirPath();
	QString filename = name_log_dir + "/test.txt";
	QFile file(filename);
	QTextStream fileText{&file};

	QByteArray datum;

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

#ifdef DEBUG_CMD_APP
			qDebug() << "V(" << i << ") = " << datum;
			sum += datum.toDouble();
			qDebug() << "SUM(V(" << (i-i) << "..." << i << ")) = " << sum;
			i++;
#endif
		}
		file.close();
	}

	return a.exec();
}
