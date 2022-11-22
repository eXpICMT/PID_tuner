#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QDir>
#include <QFile>
#include <QDate>
#include <QDebug>

#define DATABASE_HOSTNAME   "NameDataBase"
#define DATABASE_NAME       "db_pid_tuner_sql"
#define FOLDER_DATABASE     "DataUser"

#define TABLE1							"PID_Tuner"
#define TABLE1_N						"N"
#define TABLE1_U_REAL					"U_Real"
#define TABLE1_U_EST					"U_Est"
#define TABLE1_I_REAL					"I_Real"



class database : public QObject
{
	Q_OBJECT
public:
	~database();
	static database* getInstance();
	void connectToDataBase(QString path_to_base);
	bool inserIntoTable(QString table, const QVariantList &data);
	void closeDataBase();
private:
	database();
	database(const database& );
	database& operator=(const database& );
	bool openDataBase(QString file);
	bool createTable();
	bool restoreDataBase(QString file);
	QDate DataOpenLog;
public:
	QSqlDatabase db;
};

#endif // DATABASE_H
