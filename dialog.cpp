#include "dialog.h"
#include <QPushButton>
#include <QDebug>



ListFoundedItemsDialog::ListFoundedItemsDialog (QWidget* pwgt/*= 0*/)
    : QDialog (pwgt, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{

    buttonOK = new QPushButton (tr("OK"));
    buttonCancel = new QPushButton (tr("Cancel"));
    buttonSelectAll = new QPushButton (tr("SelectALL"));

    connect (buttonOK, SIGNAL(clicked()), SLOT (accept()));
    connect (buttonCancel, SIGNAL(clicked()), SLOT (reject()));

    //Layout setup
    myLayout = new QGridLayout;
    myLayout->addWidget (buttonOK, 1, 0);
    myLayout->addWidget (buttonCancel, 1, 1);

    setLayout(myLayout);
    resize(pwgt->size());
    label = new QLabel("");
}

void ListFoundedItemsDialog::setMessage(MainTableWidget **table) {

    myTable = *table;
    myLayout->addWidget(myTable, 0, 0, 1, 3);

    myLayout->addWidget(buttonSelectAll, 1, 2);
    connect(buttonSelectAll, SIGNAL(clicked()), myTable, SLOT(selectAllItems()));

}

void ListFoundedItemsDialog::setMessage(QString error) {
    label->setText(error);
    label->setAlignment(Qt::AlignCenter);
    label->setAcceptDrops(true);
    myLayout->addWidget (label, 0, 0, 1, 0);
    this->resize(300, 70);

}



