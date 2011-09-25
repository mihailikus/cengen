//дополнительные функции для QGraphicsScene
#include "graph.h"
#include <QGraphicsRectItem>

QGraphicsItem * imagelinethick(QGraphicsScene *image, float x1, float y1, float x2, float y2, int thick = 1){
    QBrush brush (Qt::black);
    QGraphicsItem* item;
     if (thick == 1) {
         item = image->addLine(x1, y1, \
                        x2, y2);
         return item;
         //imageline($image, $x1, $y1, $x2, $y2, $color);
     }
     float t = thick / 2 - 0.5;
     if ( (x1 == x2) || (y1 == y2)) {
         QGraphicsRectItem* myitem = image->addRect(round(min(x1, x2) - t), \
                        round(min(y1, y2) - t), \
                        round(max(x1, x2) + t) - (round(min(x1, x2) - t)), \
                        round(max(y1, y2) + t) - (round(min(y1, y2) - t)) );
         myitem->setBrush(brush);
         return myitem;
         /*imagefilledrectangle($image, round(min($x1, $x2) - $t), \
                                     round(min($y1, $y2) - $t), \
                                     round(max($x1, $x2) + $t), \
                                     round(max($y1, $y2) + $t), $color);
                                     */
     }

     float k = (y2 - y1) / (x2 - x1); //y = kx + q
     float a = t / sqrt(1 + pow(k, 2));
     QPolygonF polygon;
     polygon << QPointF(round(x1 - (1+k)*a), round(y1 + (1-k)*a)) \
             << QPointF(round(x1 - (1-k)*a), round(y1 - (1+k)*a)) \
             << QPointF(round(x2 + (1+k)*a), round(y2 - (1-k)*a)) \
             << QPointF(round(x2 + (1-k)*a), round(y2 + (1+k)*a));

     QGraphicsPolygonItem* pol = image->addPolygon(polygon);
     pol->setBrush(brush);


     return pol;
}

float max(float x1, float x2) {
    if (x1>x2) {
        return x1;
    } else {
        return x2;
    }
}
float min(float x1, float x2) {
    if (x1<x2) {
        return x1;
    } else {
        return x2;
    }
}

float round(float x) {
    return x;
}
