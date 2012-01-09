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


QList<Tovar> Tinformer::info(QString searchText, QString method = "tbarcode") {

    Tovar tovar;
    QList<Tovar> tovarList;
    tovar.name_of_tovar = "404 NOT FOUND";
    tovar.barcode = "404";
    tovar.nomer_of_tovar = 0;
    tovar.price1 = 0;
    tovar.price2 = 0;

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
            tovarList = dbf_info->found_record_in_dbf(searchText, method, limit_search);

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
        limit_search = 1000;
    }
}
