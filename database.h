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
#define TABLE1_U_EST_AB_FILT			"U_Est_ab_filter"
#define TABLE1_U_EST_MEDIAN_ORD_1		"U_Est_median_1st_order"
#define TABLE1_U_EST_MEDIAN_ORD_2		"U_Est_median_2nd_order"
#define TABLE1_U_EST_MEDIAN_ORD_3		"U_Est_median_3d_order"
#define TABLE1_U_EST_MEDIAN_ORD_4		"U_Est_track_du_filter_of_median"
#define TABLE1_U_EST_MEDIAN_ORD_5		"U_Est_track_du_filter_of_datum"
#define TABLE1_U_EST_MEDIAN_ORD_6		"U_Est_median_6th_order"


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
