#include "imagebox.h"

ImageBox::ImageBox() {

}

ImageBox::~ImageBox() {

}

QRectF ImageBox::imagebox(QFont font, QString textline) {
    QGraphicsScene* scene = new QGraphicsScene;
    QGraphicsTextItem* item = scene->addText(textline, font);
    return item->boundingRect();
    //delete scene;
}
