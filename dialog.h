#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QTableWidget>
#include "mainTableWidget.h"


QT_BEGIN_NAMESPACE
class QGridLayout;
class QTableWidget;
QT_END_NAMESPACE

class ListFoundedItemsDialog : public QDialog {
    Q_OBJECT
private:
    QGridLayout* myLayout;
    MainTableWidget* myTable;
    QPushButton* buttonOK;
    QPushButton* buttonCancel;
    QPushButton* buttonSelectAll;

public:
    ListFoundedItemsDialog (QWidget* pwgt = 0);

    void setMessage(MainTableWidget **table);
    void setMessage(QString error);

private slots:
    //void addTovarToList(int row, int column);
    //void selectAllItems();

};

#endif // DIALOG_H
