#ifndef DBF_INFORMER_H
#define DBF_INFORMER_H
#include "dbf.h"
#include <QFile>
#include <QMap>
#include <QTextCodec>
#include "tovar.h"

class dbf_informer : public dbfHeader {

public:
    dbf_informer();
    ~dbf_informer();

    struct dbf_one_field {
        int offset;
        int length;
    };

    QMap<QString, dbf_one_field> dbf_fields;  //для понимания полей DBF-файла


public slots:
    bool file_is_ready();
    QStringList get_dbf_header(QString filename);
    QList<Tovar> found_record_in_dbf(QString searchText, QString method, int limit,
                                     int startPos = 0, int endPos = -1,
                                     bool FromStartToEnd = true);
    QList<Tovar> found_by_tnomer(int tnomer);
    int last_found_record();

private slots:
    bool describe_dbf();
    QString get_one_cell(int offset, int lenth);
    void read_file(int startPos, int endPos);


private:
    QFile file;
    int file_read_start;
    int file_read_end;
    QTextCodec *codec;
    QTextEncoder *decoder;
    QTextDecoder *decodec;
    int last_record;
    bool first_time;
    QByteArray one_cell;

    bool first_tnomer_search;
    int maximum_tnomer;
    float *prices;
    int *offsets;


};

#endif // DBF_INFORMER_H
