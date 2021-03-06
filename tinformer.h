#ifndef TINFORMER_H
#define TINFORMER_H

//#include <QObject>
#include <QString>
//#include <QMYSQLDriver>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>
#include "tovar.h"
#include "sql_config.h"
#include "dbf_config.h"
#include "db_translator.h"
#include "dbf_informer.h"


class Tinformer {
public:
    enum DataSource { MySQL, DBF };
    enum SearchMethod { tbarcode, tnomer, tname };
    //Tinformer (int mode);
    Tinformer ();
    ~Tinformer ();

    bool prepare(SqlConfig *sql);
    bool prepare(DbfConfig *dbf);
    bool set_fields(dbTranslator *rows);
    bool set_tb_name(QString tbName);
    int get_maximum();

    QList<Tovar> info (QString text, QString method = "tbarcode",
                       int startPos = 0, int endPos = -1, int limit = 0,
                       bool FromStartToEnd = true, bool AllowFastSearch = true);
    QList<Tovar> check_line_prices (QList<Tovar> inputList);

    int last_found_record_number();
    QStringList db_describe();
    QStringList tb_describe(QString tbName);
    void set_limit_search(int number);

private:
    QSqlDatabase db;
    DataSource datasource;
    bool db_is_ready;
    bool tb_is_ready;
    SqlConfig *sqlDB;
    DbfConfig *dbfDB;
    QString values_to_select;   //  список полей для селекта из mysql
    dbTranslator* fields;       //для понимания полей SQLтаблицы
    dbf_informer* dbf_info;     //класс для работы с DBF-файлами
    int limit_search;           //ограничение поиска
    int last_record;

//public slots:


/*

public slots:

    bool is_valid();
    Tovar info(QString barcode, int method);
*/

};


#endif // TINFORMER_H
