#include "dialog.h"
#include <QLabel>
#include <QPushButton>
#include <QDebug>



MyDialog::MyDialog (QWidget* pwgt/*= 0*/)
    : QDialog (pwgt, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
    //ui_lineEdit = new QLineEdit;


    //label1->setBuddy (ui_lineEdit);

    buttonOK = new QPushButton ("OK");
    buttonCancel = new QPushButton ("Cancel");

    connect (buttonOK, SIGNAL(clicked()), SLOT (accept()));
    connect (buttonCancel, SIGNAL(clicked()), SLOT (reject()));

    //Layout setup
    myLayout = new QGridLayout;
    //myLayout->addWidget (ui_lineEdit, 0, 1);
    myLayout->addWidget (buttonOK, 1, 0);
    myLayout->addWidget (buttonCancel, 1, 1);

    setLayout(myLayout);
    //this->setFixedWidth(1000);
    this->setFixedSize(1000, 500);

}

void MyDialog::setTable(QTableWidget **table) {
    myTable = *table;
    myLayout->addWidget(myTable, 0, 0);
    connect(myTable, SIGNAL(cellClicked(int,int)), SLOT(addTovarToList(int,int)));
}

void MyDialog::setTable(QString error) {
    QLabel* label = new QLabel (error);
    label->setAlignment(Qt::AlignCenter);
    myLayout->addWidget (label, 0, 0, 1, 0);
    //myLayout->removeWidget(buttonCancel);
    //myLayout->addWidget(buttonOK, 1, 0, 1, 1);
    this->setFixedSize(300, 70);

}

void MyDialog::addTovarToList(int row, int column) {
    qDebug() << "column = " << column;
    QString text = myTable->item(row, 6)->text();
    qDebug() << text;

    if (text == "V") {
        text = " ";
    } else {
        text = "V";
    }

    //QTableWidgetItem* item = new QTableWidgetItem(text);
    //myTable->setItem(row, 6, item);
    //myTable->itemAt(row, 6)->setText(text);
    myTable->item(row, 6)->setText(text);
    //myTable->item(row, 6)->setBackgroundColor(Qt::red);
    myTable->repaint();

}


