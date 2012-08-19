#include "models/line.h"




Line::Line (float len, float angl, float LineThick) :
    QGraphicsRectItem(0, 0, len, LineThick)
{
    rotate(angl);
    itemLen = len;
    itemAngl = angl;
    itemThick = LineThick;
    qDebug() << "itemThick" << itemThick;


    itemToolBar = new QToolBar();
    setBrush(getItemBrush(ItemsTypes::Line));
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

}

Line::~Line() {}

void Line::on_size_changed() {
    this->blockSignals(true);
    rotate(0-itemAngl);
    qreal width = pwidthSpin->value() / this->sceneBoundingRect().width();
    itemLen = pwidthSpin->value();
    qreal heith = fontSizeSpin->value() / this->sceneBoundingRect().height();
    scale(width, heith);

    rotate(pheithSpin->value());
    itemAngl = pheithSpin->value();

    this->blockSignals(false);
    emit some_changed();
}

int Line::type() const {
    //qDebug() << "Type " << ItemType;
    return ItemsTypes::Line;
}

QToolBar * Line::toolBar() {

    makeToolBar(sceneBoundingRect().width(),
                sceneBoundingRect().height(),
                "");
    pwidthSpin->setValue(itemLen);
    pheithSpin->setValue(itemAngl);
    pheithSpin->setMinimum(-180);
    pheithSpin->setMaximum(180);
    fontSizeSpin->setValue(itemThick);

    connect(pheithSpin, SIGNAL(valueChanged(int)), SLOT(on_size_changed()));
    connect(pwidthSpin, SIGNAL(valueChanged(int)), SLOT(on_size_changed()));


    connect(fontSizeSpin, SIGNAL(valueChanged(int)), SLOT(on_size_changed()));
    //connect(fontButton, SIGNAL(clicked()), SLOT(on_fileButton_clicked()));


    return itemToolBar;
}

//void Line::on_fileButton_clicked() {


//    emit some_changed();
//}



