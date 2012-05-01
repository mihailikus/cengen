#include "mainTableWidget.h"
#include <QDebug>

MainTableWidget::MainTableWidget(QWidget *pwgt)
: QTableWidget (pwgt)
{
    delfield = 0;
    init();
    method = 0;
    price1=0;
    price2=0;
    set_method_view(method);
    connect(this, SIGNAL(cellClicked(int,int)), SLOT(set_editing_price2()));
    connect(this, SIGNAL(cellDoubleClicked(int,int)), SLOT(set_editing_price2()));

}

MainTableWidget::~MainTableWidget() {

}

void MainTableWidget::init() {
    shablonList.clear();
    shablonCurrent = 0;

    //подключаем слоты к сигналам
    connect(this, SIGNAL(cellClicked(int,int)), SLOT(on_tableWidget_cellClicked(int,int)));
    connect(this, SIGNAL(cellChanged(int,int)), SLOT(on_tableWidget_cellChanged(int,int)));
    connect(this, SIGNAL(itemActivated(QTableWidgetItem*)), SLOT(on_cell_entered()));

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
            if (tm1 == tr("DELETE")) {
                delfield += i;
                if (method) {
                    tm1 = tr("Check");
                }
            }
            if (tm1 == tr("Price")) {
                price1 = i;
            }
            if (tm1 == tr("Price2")) {
                price2 = i;
            }
            if (tm1 == tr("Shablon")) {
                shField = i;
            }
            if ( method && tm1 == tr("Shablon")) {
                //если у нас метод показа 1 (т.е. список найденных)
                //  то нам не нужно поле выбора шаблона, удаляем
                fieldList.remove(it.key());
                delfield--;
            } else {

                    fields << tm1;
            }

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
    tovar.shablon = shablonCurrent;
    QComboBox* box; //вынесено из цикла для работы со списком шаблонов
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
                    if (tm == tr("Quantity")) {
                        tovar.quantity = item(i, j)->text().toInt();
                    }
                    if (tm == tr("Shablon") && rowCount()<=500) {
                        box = ((QComboBox*)cellWidget(i, j));
                        tovar.shablon = box->currentIndex();
                        //qDebug() << "Shablon " << tovar.shablon;
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
    QString text;
    switch (method) {
    case 0:
        if (column == delfield) {
            this->delete_line_from_table(row);
        }
        break;
    case 1:
        text = item(row, delfield)->text();

        if (text == "V") {
            text = " ";
        } else {
            text = "V";
        }

        item(row, delfield)->setText(text);
        repaint();
        break;
    default:
        break;
    }


}

void MainTableWidget::on_tableWidget_cellChanged(int row, int column)
{
    emit data_changed();
    if (method) return; //если не 0, то ничего не редактируем
    if (editing_price2) {
        editing_price2 = false;
        tovar_searched = false;
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
    setCurrentCell(row, column);
}

void MainTableWidget::load_tovar_list_into_table(QList<Tovar> tovarList, bool ToBottom) {
    //загружаем список товаров в таблицу генератора ценников
    add_flag = true;
    if (tovarList.count()) tovar_searched = true;
    int rowsCount = rowCount();
    setRowCount(rowsCount + tovarList.count());
    QMap<QString, bool>::iterator it = fieldList.begin();
    int j = 0;
    for (; it != fieldList.end(); ++it) {
        QVariant tmp = it.key();
        if (it.value()) {
            QString tm = tmp.toString().split(QRegExp("[0-9] ")).at(1);
            if (tm == tr("Name")) {
                    for (int position = rowsCount; position<rowsCount+tovarList.count(); position++) {
                        QTableWidgetItem* item = new QTableWidgetItem(tovarList.at(position-rowsCount).name_of_tovar);
                        setItem(position, j, item);
                    }

            }
            if (tm == tr("Tnomer")) {
                for (int position = rowsCount; position<rowsCount+tovarList.count(); position++) {
                QTableWidgetItem* item = new QTableWidgetItem(QString::number(tovarList.at(position-rowsCount).nomer_of_tovar));
                setItem(position, j, item);
                }
            }
            if (tm == tr("Barcode")) {
                for (int position = rowsCount; position<rowsCount+tovarList.count(); position++) {
                QTableWidgetItem* item = new QTableWidgetItem(tovarList.at(position-rowsCount).barcode);
                setItem(position, j, item);
                }

            }
            if (tm == tr("Price")) {
                for (int position = rowsCount; position<rowsCount+tovarList.count(); position++) {
                QTableWidgetItem* item = new QTableWidgetItem(QString::number(tovarList.at(position-rowsCount).price1));
                setItem(position, j, item);
                }
            }
            if (tm == tr("Price2")) {
                for (int position = rowsCount; position<rowsCount+tovarList.count(); position++) {
                QTableWidgetItem* item = new QTableWidgetItem(QString::number(tovarList.at(position-rowsCount).price2));
                setItem(position, j, item);
                }
            }
            if (tm == tr("Quantity")) {
                for (int position = rowsCount; position<rowsCount+tovarList.count(); position++) {
                QTableWidgetItem* item = new QTableWidgetItem(QString::number(tovarList.at(position-rowsCount).quantity));
                setItem(position, j, item);
                }
            }
            if (tm == tr("DELETE")) {
                for (int position = rowsCount; position<rowsCount+tovarList.count(); position++) {

                QTableWidgetItem* item = new QTableWidgetItem(method_symbol);
                setItem(position, j, item);
                switch (method) {
                    case 0:
                        item->setToolTip(tr("DELETE"));
                        item->setWhatsThis(tr("Delete line from table"));                    break;
                    case 1:
                        item->setToolTip(tr("SELECT"));
                        item->setWhatsThis(tr("Select item to list"));                    break;
                    default:
                        break;
                    }
                }
            }
            if (tm == tr("Shablon") && tovarList.count() <= 500) {
                for (int position = rowsCount; position<rowsCount+tovarList.count(); position++) {
                QComboBox *box = new QComboBox;
                box->addItems(shablonList);
                box->setCurrentIndex(shablonCurrent);
                setCellWidget(position, j, box);
                }
            }

        j++;

        }
    }

    emit row_count_changed();
    if (ToBottom) {
        scrollToBottom();
    } else {
        scrollToTop();
    }
    add_flag = false;
    editing_price2 = true;
    tovar_searched = true;
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

void MainTableWidget::set_method_view(int method) {
    //0 - удаление с крестиком
    //1 - выбор с галочкой
    this->method = method;
    switch (method) {
    case 0:
        this->method_symbol = tr("x");
        break;
    case 1:
        this->method_symbol = tr(" ");
        itemsToSelectAll = true;
        break;
    default:
        this->method_symbol = tr("x");
        break;
    }
}

void MainTableWidget::selectAllItems() {
    if (!method) return;    //у нас таблица с удалением - нечего выделять
    QString text;
    if (itemsToSelectAll) {
        text = "V";
        itemsToSelectAll = false;
    } else {
        text = " ";
        itemsToSelectAll = true;
    }

    for (int i = 0; i<rowCount(); i++) {
        item(i, delfield)->setText(text);
    }
    repaint();
}

void MainTableWidget::set_shablon_list(QStringList shablonList) {
    this->shablonList.clear();
    this->shablonList << shablonList;
}

QStringList MainTableWidget::get_shablon_list() {
    return shablonList;
}

void MainTableWidget::set_shablon_current(int shablon) {
    this->shablonCurrent = shablon;
    if (rowCount() > 500) return;
    QComboBox* box;
    for (int i = 0; i<rowCount(); i++) {
        box = ((QComboBox*)cellWidget(i, shField));
        box->setCurrentIndex(shablon);
    }
}

void MainTableWidget::interchange_prices_in_table() {
    if (price1 == 0 || price2 == 0) {
        //одна из цен не представлена в таблице - делать нечего
        return;
    }

    QString tmp;
    for (int i = 0; i<rowCount(); i++){
            tmp = item(i, price1)->text();
            //qDebug() << "tmp " << tmp << i << p1;
            item(i, price1)->setText(item(i, price2)->text());
            item(i, price2)->setText(tmp);
    }
}

void MainTableWidget::intellect_interchange_prices_in_table() {
    if (price1 == 0 || price2 == 0) {
        //одна из цен не представлена в таблице - делать нечего
        return;
    }

    float pr1, pr2;
    for (int i = 0; i<rowCount(); i++){
        pr1 = item(i, price1)->text().toFloat();
        pr2 = item(i, price2)->text().toFloat();
        if (pr2 && pr1>pr2) {
            item(i, price1)->setText(QString::number(pr2));
            item(i, price2)->setText(QString::number(pr1));
        }
        if (!pr1) {
            item(i, price1)->setText(QString::number(pr2));
            item(i, price2)->setText("0.00");
        }
        if (pr2 == pr1) {
            item(i, price2)->setText("0.00");
        }
    }
}

void MainTableWidget::set_focus_on_price2() {
    set_editing_price2(true);
    if (rowCount()) {
        setCurrentCell(rowCount()-1, price2);
        setFocus();
    }

}

void MainTableWidget::set_special_shablon_for_zero_price2(int shablon) {
    if (!shField) return;
    if (rowCount() > 500) return;
    float pr;
    QComboBox* box;
    for (int i = 0; i<rowCount(); i++) {
        pr = item(i, price2)->text().toFloat();
        if (!pr) {
            box = ((QComboBox*)cellWidget(i, shField));
            box->setCurrentIndex(shablon);
        }
    }
}

void MainTableWidget::on_cell_entered() {
    editing_price2 = false;
    tovar_searched = false;
    emit row_count_changed();
}
