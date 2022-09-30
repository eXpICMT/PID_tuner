#include <QCoreApplication>

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QString name_log_dir = QCoreApplication::applicationDirPath();
	QString filename = name_log_dir + "/test.txt";
	QFile file(filename);
	QTextStream fileText{&file};
	string strDatum {};
	int i {};
	QByteArray datum;

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
			strDatum = datum.toStdString();
			qDebug() << datum;
			cout << "V(" << i << ") = " << strDatum << endl;
			i++;
		}
		file.close();
	}

	return a.exec();
}
