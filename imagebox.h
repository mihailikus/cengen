#ifndef IMAGEBOX_H
#define IMAGEBOX_H

//определение размера текста на сцене

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QFont>
#include <QString>
#include <QRectF>


class ImageBox {
public:
    ImageBox();
    ~ImageBox();

public slots:
    QRectF imagebox(QFont font, QString textline);

};

#endif // IMAGEBOX_H
