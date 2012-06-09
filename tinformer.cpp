#include "tinformer.h"
#include <QDebug>


/*
Tinformer::Tinformer(int mode) {
    this->datasource = mode;

}
*/
Tinformer::Tinformer() {

    this->db_is_ready = false;
    this->tb_is_ready = false;
    this->limit_search = 10;
    fields = new dbTranslator;
    last_record = 0;

}

Tinformer::~Tinformer() {
    delete dbf_info;
}

bool Tinformer::prepare(SqlConfig *sql) {

    this->datasource = MySQL;
    this->sqlDB = sql;
    //устанавливаем соединение с базой данных
    //qDebug() << "Connection to mysql";
    db = QSqlDatabase::addDatabase("QMYSQL", "my_mysql");
    //qDebug() << "settings 555";
    db.setHostName(sqlDB->dbHost);
    db.setDatabaseName(sqlDB->dbName);
    db.setUserName(sqlDB->dbUser);
    db.setPassword(sqlDB->dbPass);
    //qDebug() << "settings 8989";

    if (db.open()) {
        this->db_is_ready = true;
        qDebug() << "connection established";

    } else {
        this->db_is_ready = false;
    }

    return db_is_ready;
}

bool Tinformer::prepare(DbfConfig *dbf) {
    this->datasource = DBF;
    this->dbfDB = dbf;
    //открываем файл, читаем описатели
    this->dbf_info = new dbf_informer();

    //qDebug() << "will parse DBF file" << dbfDB->fileName;

    this->db_is_ready = dbf_info->file_is_ready();
    //qDebug() << "DBF file is " << db_is_ready;

    return db_is_ready;
}


QList<Tovar> Tinformer::info(QString searchText, QString method,
                             int startPos, int endPos, int limit,
                             bool FromStartToEnd,
                             bool AllowFastSearch) {

    Tovar tovar;
    QList<Tovar> tovarList;
    tovar.name_of_tovar = "404 NOT FOUND";
    tovar.barcode = "404";
    tovar.nomer_of_tovar = 0;
    tovar.price1 = 0;
    tovar.price2 = 0;
    tovar.shablon = 0;
    tovar.quantity = 0;

    if (!limit) limit = limit_search;

    if (method == "any") {
        //не для поиска, а для создания пустой строки
        tovar.name_of_tovar = "";
        tovar.barcode = "0";
        tovarList << tovar;
        return tovarList;
    }

    if ( (datasource == Tinformer::MySQL) && \
        this->tb_is_ready && \
        this->db_is_ready) {
        QString query;
        //qDebug() << "started OK";
        query = "SELECT " + fields->tnomer + ", " \
                + fields->tname + ", "\
                + fields->tbarcode + ", "\
                + fields->tprice + \
                + " FROM `" \
                + sqlDB->dbName + "`.`" \
                + sqlDB->tbName + "`"\
                + " WHERE `";
        if (method == "tbarcode") {
           query += fields->tbarcode + "` = " + searchText;
        }
        if (method == "tnomer") {
            query += fields->tnomer + "` = " + searchText;
        }
        if (method == "tname") {
            query += fields->tname + "` LIKE '%" + searchText + "%' LIMIT " +
                    QString::number(this->limit_search);
        }

        qDebug() << "query = " << query;
        QSqlQuery res = db.exec(query);
        while (res.next()) {
            QString price = res.value(3).toString();
            if (price.contains(",")) {
                QString rub = price.split(",").at(0);
                QString kop = price.split(",").at(1);
                tovar.price1 = rub.toFloat() + kop.toFloat()/100;
            } else {
                tovar.price1 = res.value(3).toFloat();
            }
            tovar.nomer_of_tovar = res.value(0).toInt();
            tovar.name_of_tovar = res.value(1).toString().trimmed();
            tovar.barcode = res.value(2).toString().trimmed();
            tovar.price2 = 0;
            tovarList << tovar;

        }


    }

    if (datasource == Tinformer::DBF) {
        if (this->tb_is_ready) {

            if (AllowFastSearch && method == "tnomer" ) {
                tovarList = dbf_info->found_by_tnomer(searchText.toInt());
                last_record = 0;
            } else {
                tovarList = dbf_info->found_record_in_dbf(searchText, method, limit,
                                                          startPos, endPos,
                                                          FromStartToEnd);
                last_record = dbf_info->last_found_record();
            }
        }


    }
    //SELECT * FROM неведомая фигня
    return tovarList;
}

QStringList Tinformer::db_describe() {
    //возвращает список таблиц
    QStringList list;
    if (db_is_ready) {
        QSqlQuery res = db.exec("SHOW TABLES");
        while (res.next()) {
            QString tmp = res.value(0).toString();
            //qDebug() << "TD " << tmp;
            list << tmp;
        }

    }
    return list;
}

QStringList Tinformer::tb_describe(QString tbName = "DBF") {
    //возвращает список полей в выбранной таблице
    QStringList list;
    if (datasource == Tinformer::MySQL) {
        if (db_is_ready) {
            QSqlQuery res = db.exec("DESCRIBE `" + tbName + "`");
            while (res.next()) {
                QString tmp = res.value(0).toString();
                //qDebug () << tmp;
                list << tmp;
            }
        }
    }
    if (datasource == Tinformer::DBF) {
        list = dbf_info->get_dbf_header(this->dbfDB->fileName);
    }


    return list;
}

bool Tinformer::set_fields(dbTranslator *rows) {
    if (datasource == Tinformer::MySQL) {
        this->fields = rows;
        qDebug() << "using function set_fields at tinformer" ;
        //qDebug() << fields->tname;
        this->tb_is_ready = true;
        return true; //а нафига?
    }
    if (datasource == Tinformer::DBF) {
        //для каждого имени в таблице, указывающего на тов. номер, штрихкод и т.п.
        //  указываем смещение относительно начала записи в DBF-файле
        QStringList names, values;
        names << "tbarcode" << "tnomer" << "tname" << "tprice";
        values << rows->tbarcode << rows->tnomer << rows->tname << rows->tprice;
        dbf_info->dbf_fields.clear();

        for (int j = 0; j<names.count(); j++) {
            //qDebug() << "search for field " << values.at(j);
            int offset =0;
            for (int i = 0; i < dbf_info->fields.count(); i++) {
                if (dbf_info->fields.at(i).name == values.at(j)) {
                    //qDebug() << "found " << dbf_info->fields.at(i).name;
                    dbf_info->dbf_fields[names.at(j)].offset = offset;
                    //qDebug() << "offset is " << offset;
                    dbf_info->dbf_fields[names.at(j)].length = dbf_info->fields.at(i).length;

                    //qDebug() << "writing field " << names.at(j);
                }
                offset += dbf_info->fields.at(i).length;
            }

        }

        //qDebug() << "Offset for barcode = " << dbf_info->dbf_fields[names.at(0)].offset;
        //qDebug() << "Length for barcode = " << dbf_info->dbf_fields[names.at(0)].length;

        this->tb_is_ready = true;
        return true;
    }
    return false;
}

bool Tinformer::set_tb_name(QString tbName) {
    if (datasource == Tinformer::MySQL) {
        this->sqlDB->tbName = tbName;
        this->tb_is_ready = true;
        return true;
    }
    if (datasource == Tinformer::DBF) {
        this->dbfDB->fileName = tbName;
        this->db_is_ready = true;
        this->tb_is_ready = true;
        return true;
    }
    return false;

}

void Tinformer::set_limit_search(int number) {
    if (number) {
        limit_search = number;
    } else {
        limit_search = get_maximum();
    }
}

int Tinformer::get_maximum() {
    if (!db_is_ready) return -1;
    switch (datasource) {
    case MySQL:
        return 1000;
        break;
    case DBF:
        return this->dbf_info->number_of_records;
        break;
    default:
        return -1;
        break;
    }
}

int Tinformer::last_found_record_number() {
    return last_record;
}

QList<Tovar> Tinformer::check_line_prices(QList<Tovar> inputList) {
    QList<Tovar> outList;
    if (inputList.count()<3) {
        return outList;
    }
    //для сравнения цен:
    float pr1, pr2, tmp;

    //подсчет разных цен:
    int c1, c2;

    //список для сохранения товарных номеров списка цен
    QList<int> t1, t2;
    c1 = 0;
    c2 = 0;
    pr1 = inputList.at(0).price1;
    pr2 = inputList.at(1).price1;
    t1 << inputList.at(0).nomer_of_tovar;
    int bardak = 0;

    for (int i = 1; i<inputList.count(); i++) {
        tmp = inputList.at(i).price1;
        if (pr1 == tmp) {
            t1 << inputList.at(i).nomer_of_tovar;
           c1++;
        } else {
            if (pr2 == tmp) {
                c2 ++;
                t2 << inputList.at(i).nomer_of_tovar;
            } else {
                bardak++;
                pr2 = tmp;
                c2++;
                t2 << inputList.at(i).nomer_of_tovar;
            }

        }
    }
    if (!bardak) return outList;

    if (bardak<2) {
        //если разных цен не больше двух
        Tovar tovar;
        tovar.nomer_of_tovar = 0;
        tovar.barcode = "";
        tovar.price1 = pr1;
        tovar.price2 = pr2;
        tovar.quantity = c1;
        tovar.name_of_tovar = inputList.at(0).name_of_tovar;
        QString desc;
        if (c1 > c2) {
            desc = "All by " + QString::number(pr1) + ", but ";
            for (int i = 0; i<t2.count(); i++) {
                desc += QString::number(t2.at(i)) + ", ";
            }
            desc += " by " + QString::number(pr2);
        } else {
            desc = "All by " + QString::number(pr2) + ", but ";
            for (int i = 0; i<t1.count(); i++) {
                desc += QString::number(t1.at(i)) + ", ";
            }
            desc += " by " + QString::number(pr1);
        }
        tovar.name_of_tovar += ": " + desc;
        outList << tovar;
    } else {
        //разных цен три и более
        Tovar tovar;
        tovar.name_of_tovar = "Many different prices: " + inputList.at(0).name_of_tovar;
        outList << tovar;
    }


    return outList;
}
