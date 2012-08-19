#include "models/image.h"




Image::Image (float w, float h, QString fileName) :
    QGraphicsRectItem(0, 0, w, h)
{
    itemToolBar = new QToolBar();
    setBrush(getItemBrush(ItemsTypes::Image));
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    itemText = fileName;

}

Image::~Image() {}

void Image::on_sizeX_changed() {
    qreal width = pwidthSpin->value() / this->sceneBoundingRect().width();
    //qreal heith = pheithSpin->value() / this->sceneBoundingRect().height();
    scale(width, width);
    pheithSpin->blockSignals(true);
    pheithSpin->setValue(pheithSpin->value()*width);
    pheithSpin->blockSignals(false);
}

void Image::on_sizeY_changed() {
    //qreal width = pwidthSpin->value() / this->sceneBoundingRect().width();
    qreal heith = pheithSpin->value() / this->sceneBoundingRect().height();
    scale(heith, heith);
    pwidthSpin->blockSignals(true);
    pwidthSpin->setValue(pwidthSpin->value()*heith);
    pwidthSpin->blockSignals(false);
}
int Image::type() const {
    //qDebug() << "Type " << ItemType;
    return ItemsTypes::Image;
}

QToolBar * Image::toolBar() {

    makeToolBar(sceneBoundingRect().width(),
                sceneBoundingRect().height(),
                "FILE");

    connect(pheithSpin, SIGNAL(valueChanged(int)), SLOT(on_sizeY_changed()));
    connect(pwidthSpin, SIGNAL(valueChanged(int)), SLOT(on_sizeX_changed()));

    connect(fontButton, SIGNAL(clicked()), SLOT(on_fileButton_clicked()));


    return itemToolBar;
}

void Image::on_fileButton_clicked() {

    QString str = QFileDialog::getOpenFileName(0, tr("Select image file"), itemText, tr("images (*.jpg *.gif)"));

    itemTextLine->setText(str);
    itemText = str;

    emit some_changed();
}


