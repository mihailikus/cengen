#ifndef MAINTABLEWIDGET_H
#define MAINTABLEWIDGET_H

#include <QTableWidget>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMap>
#include <QComboBox>
#include "tovar.h"

class MainTableWidget : public QTableWidget {
    Q_OBJECT

public:
    MainTableWidget(QWidget* pwgt = 0);
    ~MainTableWidget();

    void set_add_flag(bool status);
    void set_tovar_searched(bool status);
    bool is_tovar_searched();
    bool is_editing_price2();

    QList<Tovar> get_tovar_list(QString priznak);
    void load_tovar_list_into_table(QList<Tovar> tovarList, bool ToBottom = true);
    int get_tableTab_width(int col);
    void set_tableTab_width(int col, int size);

    void set_tableFields(QMap<QString, bool> list);

    void set_method_view(int method);

    void set_shablon_list(QStringList shablonList);
    QStringList get_shablon_list();
    void set_shablon_current(int shablon);

    void set_special_shablon_for_zero_price2(int shablon);

    void set_focus_on_price2();


public slots:
    void selectAllItems();
    void interchange_prices_in_table();
    void intellect_interchange_prices_in_table();
    void set_editing_price2(bool status = false);
    long double sum_of_tovar();
    void remove_zero_quantity();

private:
    QHeaderView* tableHeader;
    bool editing_price2;
    bool add_flag;
    bool tovar_searched;
    QMap<QString, bool> fieldList;

    QStringList fields; //перечислены те поля, которые используются
    int delfield;   //поле, нажатие на которое означает удаление
    int shField;    //поле, в которое (возможно) будет заноситься список шаблонов
    int method;     //указывает, будет таблица с флагом удаления или с выбором
    QString method_symbol;  //галочка или крестик
    bool itemsToSelectAll;

    QStringList shablonList;
    int shablonCurrent;

    int price1, price2; //порядковые номера, где цена1, где старая цена
    int quantity;


private slots:
    void init();


    void on_tableWidget_cellClicked(int,int);
    void on_tableWidget_cellChanged(int,int);
    void delete_line_from_table(int pos);
    //void add_table_item(int position, Tovar tovar);
    void on_cell_entered ();


signals:
    void row_count_changed();
    void data_changed();

};


#endif // MAINTABLEWIDGET_H
