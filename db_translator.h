#ifndef DB_TRANSLATOR_H
#define DB_TRANSLATOR_H
//используется для понимания, какое из полей в базе данных отвечает за номер, название и т.п.


#include <QString>

class dbTranslator {
public:
    QString tnomer;
    QString tname;
    QString tbarcode;
    QString tprice;
};


#endif // DB_TRANSLATOR_H
