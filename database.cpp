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
		query.prepare("INSERT INTO " TABLE1 " ( "   TABLE1_U_REAL				", "		//0
													TABLE1_U_EST				", "		//1
													TABLE1_I_REAL				" ) "
					  "VALUES (:F1, :F2, :F3)");

		query.bindValue(":F1",       data[0]);
		query.bindValue(":F2",       data[1]);
		query.bindValue(":F3",       data[2]);
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
					TABLE1_U_REAL					" NUMERIC    NOT NULL,"
					TABLE1_U_EST					" NUMERIC    NOT NULL,"
					TABLE1_I_REAL					" NUMERIC    NOT NULL"
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
