#ifndef DBF_H
#define DBF_H

#include <QList>
#include <QTextCodec>

struct dbf_field_descr {
    QString name;
    QChar type;
    int length;
    int decimal_count;
    int index_field_flag;
};

struct dbfHeader {
    //переменные-описатели DBF-файла
    QString date;               //дата изменения файла
    int data_position_in_file;
    int number_of_records;
    int length_of_header_structure;
    int length_of_each_record;
    int incomplete_transac;

    //список с описателями полей полученного DBF-файла
    QList<dbf_field_descr> fields;

    //массив для хранения всего файла
    char *all_records;

    QTextCodec *codec;      //для перекодирования

};

#endif // DBF_H
