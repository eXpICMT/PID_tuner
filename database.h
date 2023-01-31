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
#define DATABASE_NAME       "db_ekf_research_sql"
#define FOLDER_DATABASE     "DataUser"

#define TABLE1                              "PID_Tuner"
#define TABLE1_N                            "N"
#define TABLE1_I_REAL                       "I_Real"
#define TABLE1_U_REAL                       "U_Real"
#define TABLE1_U_lineal_SUN_model			"U_lineal_SUN_model"
#define TABLE1_U_result_R_0_005             "U_result_kf"
#define TABLE1_U_result_R_0_05              "SOC_from_OCV"
#define TABLE1_U_result_R_0_5               "NA_2"
#define TABLE1_U_result_R_5                 "NA_3"




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
