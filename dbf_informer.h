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
    int last_found_record();

private slots:
    bool describe_dbf();
    QString get_one_cell(int offset, int lenth);


private:
    QFile file;
    int file_read_start;
    int file_read_end;
    QTextCodec *codec;
    int last_record;
    bool first_time;






};

#endif // DBF_INFORMER_H
