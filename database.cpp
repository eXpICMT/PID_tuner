#include "database.h"

database::database()
{
}

database* database::getInstance()
{
	static database instance;
	return &instance;
}

database::~database()
{
}

void database::connectToDataBase(QString path_to_base)
{
	QString NAME_DIR = path_to_base;
	if(!QDir(NAME_DIR + "/" + FOLDER_DATABASE).exists())
	{
		qDebug() << "Folder for Database wasn't found!";
		QDir().mkpath(NAME_DIR + "/" + FOLDER_DATABASE);
	}
	NAME_DIR = NAME_DIR + "/" + FOLDER_DATABASE + "/";
	NAME_DIR += DATABASE_NAME;
	QDateTime dateTime = QDateTime::currentDateTime();
	NAME_DIR += dateTime.toString("MM_yy");
	NAME_DIR += ".db";
	if(!QFile(NAME_DIR).exists())
	{
		qDebug() << "Database file wasn't found!";
		this->restoreDataBase(NAME_DIR);

	}else
	{
		this->openDataBase(NAME_DIR);
	}
}

bool database::inserIntoTable(QString table, const QVariantList &data)
{
	QSqlQuery query("BaseQuery", db);
	if(table == TABLE1)
	{
        query.prepare("INSERT INTO " TABLE1 " ( "   TABLE1_I_REAL                   ", "		//  0
                                                    TABLE1_U_REAL                   ", "		//  1
                                                    TABLE1_U_lineal_SUN_model       ", "		//  2
                                                    TABLE1_U_result_R_0_005     	", "        //  3
                                                    TABLE1_U_result_R_0_05     		", "        //  4
                                                    TABLE1_U_result_R_0_5     		", "        //  5
                                                    TABLE1_U_result_R_5     		", "        //  6
                                                    TABLE1_U_ekf_result             ", "        //  7
                                                    TABLE1_Err_non_linear           ", "        //  8
                                                    TABLE1_OCV_5_order              ", "        //  9
                                                    TABLE1_I_PID                    ", "        //  10
                                                    TABLE1_U_PID                    ", "        //  11
                                                    TABLE1_U_SRCPID                 ", "        //  12
                                                    TABLE1_I_EKFPID                 ", "        //  13
                                                    TABLE1_U_EKFPID                 ", "        //  14
                                                    TABLE1_USRC_EKFPID              " ) "       //  15
                      "VALUES (:F1, :F2, :F3, :F4, :F5, :F6, :F7, :F8, :F9, :F10, :F11, :F12, :F13, :F14, :F15, :F16)");

		query.bindValue(":F1",       data[0]);
		query.bindValue(":F2",       data[1]);
		query.bindValue(":F3",       data[2]);
        query.bindValue(":F4",       data[3]);
        query.bindValue(":F5",       data[4]);
        query.bindValue(":F6",       data[5]);
        query.bindValue(":F7",       data[6]);
        query.bindValue(":F8",       data[7]);
        query.bindValue(":F9",       data[8]);
        query.bindValue(":F10",      data[9]);
        query.bindValue(":F11",      data[10]);
        query.bindValue(":F12",      data[11]);
        query.bindValue(":F13",      data[12]);
        query.bindValue(":F14",      data[13]);
        query.bindValue(":F15",      data[14]);
        query.bindValue(":F16",      data[15]);
	}
	else
	{
		qDebug() << "no table selected ";
		return false;
	}

	if(!query.exec())
	{
		qDebug() << "error of inserting into the database";
		qDebug() << query.lastError().text();
		return false;
	} else {
		return true;
	}
	return false;
}

void database::closeDataBase()
{
	db.close();
}

bool database::openDataBase(QString file)
{
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setHostName(DATABASE_HOSTNAME);
	db.setDatabaseName(file);
	if(db.open())
	{
		qDebug() << "Database was opened!";
		return true;
	} else
	{
		qDebug() << "Database wasn't opened!";
		return false;
	}
}

bool database::createTable()
{
	QSqlQuery query("BaseQuery", db);
	if(!query.exec( "CREATE TABLE " TABLE1 " ("
					"N INTEGER PRIMARY KEY AUTOINCREMENT, "
                    TABLE1_I_REAL                       " NUMERIC    NOT NULL,"
                    TABLE1_U_REAL                       " NUMERIC    NOT NULL,"
                    TABLE1_U_lineal_SUN_model			" NUMERIC    NOT NULL,"
                    TABLE1_U_result_R_0_005             " NUMERIC    NOT NULL,"
                    TABLE1_U_result_R_0_05              " NUMERIC    NOT NULL,"
                    TABLE1_U_result_R_0_5               " NUMERIC    NOT NULL,"
                    TABLE1_U_result_R_5                 " NUMERIC    NOT NULL,"
                    TABLE1_U_ekf_result                 " NUMERIC    NOT NULL,"
                    TABLE1_Err_non_linear               " NUMERIC    NOT NULL,"
                    TABLE1_OCV_5_order                  " NUMERIC    NOT NULL,"
                    TABLE1_I_PID                        " NUMERIC    NOT NULL,"
                    TABLE1_U_PID                        " NUMERIC    NOT NULL,"
                    TABLE1_U_SRCPID                     " NUMERIC    NOT NULL,"
                    TABLE1_I_EKFPID                     " NUMERIC    NOT NULL,"
                    TABLE1_U_EKFPID                     " NUMERIC    NOT NULL,"
                    TABLE1_USRC_EKFPID                  " NUMERIC    NOT NULL"
                    " )"
					))
	{
		qDebug() << "DataBase: error of create " << TABLE1;
		qDebug() << query.lastError().text();
		return false;
	}
	else
	{
		return true;
	}
	return false;
}



bool database::restoreDataBase(QString file)
{
	if(this->openDataBase(file))
	{
		return (this->createTable()) ? true : false;
	} else
	{
		qDebug() << "Could not restore database";
		return false;
	}
	return false;
}
