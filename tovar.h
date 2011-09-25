#ifndef TOVAR_H
#define TOVAR_H
#include <QString>
struct Tovar {
    //переменные для идентификации товара:
    QString name_of_tovar;
    float price1, price2;
    QString barcode;
    int nomer_of_tovar; //товарный номер
    QString codec_name;
    //static int const organization = 34;
};
#endif // TOVAR_H
