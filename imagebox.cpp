#include "imagebox.h"

ImageBox::ImageBox() {

}

ImageBox::~ImageBox() {

}

QRectF ImageBox::imagebox(QFont font, QString textline, QGraphicsScene *scene) {
    QGraphicsTextItem* item = scene->addText(textline, font);
    QRectF rect = item->boundingRect();
    delete item;
    return rect;
}
