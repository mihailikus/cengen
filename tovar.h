#ifndef TOVAR_H
#define TOVAR_H
#include <QString>
///переменные для идентификации товара:
struct Tovar {
    ///название товара
    QString name_of_tovar;
    ///текущая цена
    float price1;
    ///старая цена
    float price2;
    ///штрих-код товара
    QString barcode;
    ///товарный номер
    int nomer_of_tovar;
    ///языковой кодек
    QString codec_name;
    ///номер шаблона из списка шаблонов в выбранной дирректории
    int shablon;
    ///количество
    int quantity;
    ///оператор сравнения двух товаров. Возвращает TRUE, если количество текущего товара меньше, чем номер количество другого
    bool operator < (const Tovar &other) const
      {
        if (quantity < other.quantity)
          return true;

        return false;
    }
};
#endif // TOVAR_H
