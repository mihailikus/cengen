#include "dialog.h"
#include <QLabel>
#include <QPushButton>
#include <QDebug>



ListFoundedItemsDialog::ListFoundedItemsDialog (QWidget* pwgt/*= 0*/)
    : QDialog (pwgt, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{

    buttonOK = new QPushButton (tr("OK"));
    buttonCancel = new QPushButton (tr("Cancel"));
    buttonSelectAll = new QPushButton (tr("SelectALL"));

    itemsToSelectAll = true;

    connect (buttonOK, SIGNAL(clicked()), SLOT (accept()));
    connect (buttonCancel, SIGNAL(clicked()), SLOT (reject()));

    //Layout setup
    myLayout = new QGridLayout;
    myLayout->addWidget (buttonOK, 1, 0);
    myLayout->addWidget (buttonCancel, 1, 1);

    setLayout(myLayout);
    this->setFixedSize(1000, 500);

}

void ListFoundedItemsDialog::setTable(MainTableWidget **table) {
    myTable = *table;
    myLayout->addWidget(myTable, 0, 0, 1, 3);
    connect(myTable, SIGNAL(cellClicked(int,int)), SLOT(addTovarToList(int,int)));

    myLayout->addWidget(buttonSelectAll, 1, 2);
    connect(buttonSelectAll, SIGNAL(clicked()), SLOT(selectAllItems()));

}

void ListFoundedItemsDialog::setTable(QString error) {
    QLabel* label = new QLabel (error);
    label->setAlignment(Qt::AlignCenter);
    myLayout->addWidget (label, 0, 0, 1, 0);
    this->setFixedSize(300, 70);

}

void ListFoundedItemsDialog::addTovarToList(int row, int column) {
    //qDebug() << "column = " << column;
    QString text = myTable->item(row, 6)->text();
    //qDebug() << text;

    if (text == "V") {
        text = " ";
    } else {
        text = "V";
    }

    myTable->item(row, 6)->setText(text);
    myTable->repaint();

}

void ListFoundedItemsDialog::selectAllItems() {
    QString text;
    if (itemsToSelectAll) {
        text = "V";
        itemsToSelectAll = false;
    } else {
        text = " ";
        itemsToSelectAll = true;
    }

    for (int i = 0; i<myTable->rowCount(); i++) {
        myTable->item(i, 6)->setText(text);
    }
    myTable->repaint();
}

