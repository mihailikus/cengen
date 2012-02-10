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

    void set_editing_price2(bool status);
    void set_add_flag(bool status);
    void set_tovar_searched(bool status);
    bool is_tovar_searched();
    bool is_editing_price2();

    QList<Tovar> get_tovar_list(QString priznak);
    void load_tovar_list_into_table(QList<Tovar> tovarList);
    void add_table_item(int position, Tovar tovar);
    int get_tableTab_width(int col);
    void set_tableTab_width(int col, int size);

    void set_tableFields(QMap<QString, bool> list);

    void set_method_view(int method);

    void set_shablon_list(QStringList shablonList);
    void set_shablon_current(int shablon);

public slots:
    void selectAllItems();

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




private slots:
    void init();


    void on_tableWidget_cellClicked(int,int);
    void on_tableWidget_cellChanged(int,int);
    void delete_line_from_table(int pos);



signals:
    void row_count_changed();

};


#endif // MAINTABLEWIDGET_H
