#include "models/abstract_item.h"
#include <QDebug>


void AbstractItem::setText(QString text) {
    itemText = text;
}

void AbstractItem::makeToolBar(int w, int h, QString buttonText) {
    delete itemToolBar;
    itemToolBar = new QToolBar;

    pwidthSpin = new QSpinBox;
    pwidthSpin->setMinimum(1);
    pwidthSpin->setMaximum(1000);

    pheithSpin = new QSpinBox;
    pheithSpin->setMinimum(1);
    pheithSpin->setMaximum(1000);

    pheithSpin->setValue(h);
    pwidthSpin->setValue(w);

    fontSizeSpin = new QSpinBox;
    fontSizeSpin->setMinimum(0);
    fontSizeSpin->setMaximum(1000);
    fontSizeSpin->setValue(itemFontSize);

    itemToolBar->addWidget(pwidthSpin);
    itemToolBar->addWidget(pheithSpin);
    itemToolBar->addWidget(fontSizeSpin);


    if (buttonText != "") {
        fontButton = new QPushButton(buttonText);
        itemToolBar->addWidget(fontButton);
        itemTextLine = new QLineEdit();
        itemTextLine->setText(itemText);
        itemToolBar->addWidget(itemTextLine);
    }


}


void AbstractItem::setFont(int fontSize, QString fontFamily, bool Bold, bool Italic) {
    itemFont.setFamily(fontFamily);
    itemFont.setPixelSize(fontSize);
    itemFont.setBold(Bold);
    itemFont.setItalic(Italic);
    itemFontSize = fontSize;
}

QBrush AbstractItem::getItemBrush(int tpt) {
    QBrush brush;
    brush = QBrush(Qt::magenta);
    //qDebug() <<"method: " << method << tpt;
    if (tpt == ItemsTypes::Barcode) return QBrush(Qt::darkBlue);

    if (tpt == ItemsTypes::textInBox) {
        if (method == "price") return QBrush(Qt::red);
        if (method == "priceOld") return QBrush(Qt::darkRed);
        if (method == "textInBox") return QBrush(Qt::yellow);
        if (method == "date") return QBrush(Qt::lightGray);
        if (method == "nomer") return QBrush(Qt::darkGreen);
        if (method == "barcodeAsText") return QBrush(Qt::darkMagenta);
        if (method == "quantity") return QBrush(Qt::cyan);
    }
    if (tpt == ItemsTypes::Good) return QBrush(Qt::green);

    if (tpt == ItemsTypes::Line) return QBrush(Qt::black);

    ItemType = tpt;

    return brush;

}
