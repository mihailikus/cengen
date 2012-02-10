#include "mainTableWidget.h"
#include <QDebug>

MainTableWidget::MainTableWidget(QWidget *pwgt)
: QTableWidget (pwgt)
{
    delfield = 255;
    init();
}

MainTableWidget::~MainTableWidget() {

}

void MainTableWidget::init() {
   //verticalHeader()->hide();
    //подключаем слоты к сигналам
    connect(this, SIGNAL(cellClicked(int,int)), SLOT(on_tableWidget_cellClicked(int,int)));
    connect(this, SIGNAL(cellChanged(int,int)), SLOT(on_tableWidget_cellChanged(int,int)));

}

void MainTableWidget::set_tableFields(QMap<QString, bool> list) {
    fieldList = list;
    QMultiMap<QString, bool>::iterator it = list.begin();
    QStringList tmp;
    fields.clear();
    int i = 0;
    for (; it != list.end(); ++it) {
        if (it.value()) {
            tmp << it.key();
            QString tm = tmp.at(i);
            QString tm1 = tm.split(QRegExp("[0-9] ")).at(1);
            if (tm.split(QRegExp(" ")).at(0).toInt() == 8) {
                delfield = i;
            }
            fields << tm1;
            i++;
        }
    }
    setRowCount(0);
    setColumnCount(fields.count());

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    //устанавливаем заголовки главной таблицы
    QStandardItemModel *model = new QStandardItemModel(5,5, this);
    QTableView *tableView = new QTableView;
    tableView->setModel(model);
    tableHeader = new QHeaderView(Qt::Horizontal, tableView);
    QStandardItemModel *header_model_1 = new QStandardItemModel(0,0, tableHeader);
    header_model_1->setHorizontalHeaderLabels(fields);

    tableHeader->setModel(header_model_1);
    setHorizontalHeader(tableHeader);
}

QList<Tovar> MainTableWidget::get_tovar_list(QString priznak) {
    QList<Tovar> spisok;
    Tovar tovar;
    for (int i = 0; i<rowCount(); i++) {
        if (item(i, delfield)->text() == priznak) {

            QMap<QString, bool>::iterator it = fieldList.begin();
            int j = 0;
            for (; it != fieldList.end(); ++it) {
                QVariant tmp = it.key();
                if (it.value()) {
                    QString tm = tmp.toString().split(QRegExp("[0-9] ")).at(1);
                    if (tm == tr("Name")) {
                        tovar.name_of_tovar = item(i, j)->text().trimmed();

                    }
                    if (tm == tr("Tnomer")) {
                        tovar.nomer_of_tovar = item(i, j)->text().toInt();

                    }
                    if (tm == tr("Barcode")) {
                        tovar.barcode = item(i, j)->text().trimmed();

                    }
                    if (tm == tr("Price")) {
                        tovar.price1 = item(i, j)->text().toFloat();

                    }
                    if (tm == tr("Price2")) {
                        if (!item(i,j)->text().isEmpty()) {
                            tovar.price2 = item(i, j)->text().toFloat();
                        }

                    }

                    j++;
                }
            }
            spisok << tovar;
        }
    }
    return spisok;
}

void MainTableWidget::delete_line_from_table(int pos) {
    removeRow(pos);
    emit row_count_changed();
}

void MainTableWidget::on_tableWidget_cellClicked(int row, int column)
{
    if (column == delfield) {
        this->delete_line_from_table(row);
    }
}

void MainTableWidget::on_tableWidget_cellChanged(int row, int column)
{
    if (editing_price2) {
        editing_price2 = false;
        emit row_count_changed();
        return;
    }
    if (add_flag) {
        return;
    }
    if (row < rowCount()-1) {
        row++;
    } else {
        if (column < columnCount() -1) {
            column++;
        }
    }
    QTableWidgetItem* itemC = item(row, column);
    setCurrentItem(itemC);
}

void MainTableWidget::add_table_item(int position, Tovar tovar) {
    QMap<QString, bool>::iterator it = fieldList.begin();
    int j = 0;
    for (; it != fieldList.end(); ++it) {
        QVariant tmp = it.key();
        if (it.value()) {
            QString tm = tmp.toString().split(QRegExp("[0-9] ")).at(1);
            if (tm == tr("Name")) {
                QTableWidgetItem* item = new QTableWidgetItem(tovar.name_of_tovar);
                setItem(position, j, item);
            }
            if (tm == tr("Tnomer")) {
                QTableWidgetItem* item = new QTableWidgetItem(QString::number(tovar.nomer_of_tovar));
                setItem(position, j, item);
            }
            if (tm == tr("Barcode")) {
                QTableWidgetItem* item = new QTableWidgetItem(tovar.barcode);
                setItem(position, j, item);

            }
            if (tm == tr("Price")) {
                QTableWidgetItem* item = new QTableWidgetItem(QString::number(tovar.price1));
                setItem(position, j, item);
            }
            if (tm == tr("Price2")) {
                QTableWidgetItem* item = new QTableWidgetItem(QString::number(tovar.price2));
                setItem(position, j, item);
            }
            if (tm == tr("DELETE")) {
                QTableWidgetItem* itemDel = new QTableWidgetItem("x");
                setItem(position, j, itemDel);
                item(position, j)->setToolTip(tr("DELETE"));
                item(position, j)->setWhatsThis(tr("Delete line from table"));
            }
            j++;
        }

    }


}

void MainTableWidget::load_tovar_list_into_cengen(QList<Tovar> tovarList) {
    //загружаем список товаров в таблицу генератора ценников
    add_flag = true;
    if (tovarList.count()) tovar_searched = true;
    for (int i = 0; i<tovarList.count(); i++) {
        Tovar tovar = tovarList.at(i);

        int position = rowCount();
        setRowCount(position + 1);
        emit row_count_changed();

        add_table_item(position, tovar);


    }
    scrollToBottom();
    add_flag = false;
}

void MainTableWidget::set_editing_price2(bool status) {
    editing_price2 = status;
}

void MainTableWidget::set_add_flag(bool status) {
    add_flag = status;
}

void MainTableWidget::set_tovar_searched(bool status) {
    tovar_searched = status;
}

bool MainTableWidget::is_tovar_searched() {
    return tovar_searched;
}

bool MainTableWidget::is_editing_price2() {
    return editing_price2;
}

int MainTableWidget::get_tableTab_width(int col) {
    return horizontalHeader()->sectionSize(col);
}

void MainTableWidget::set_tableTab_width(int col, int size) {
    horizontalHeader()->resizeSection(col, size);
}
