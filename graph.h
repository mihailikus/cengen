#ifndef GRAPH_H
#define GRAPH_H
//This is used for draw thick line
//is deprecated and will be removed
#include <QGraphicsScene>
#include <math.h>

QGraphicsItem * imagelinethick(QGraphicsScene * image, \
                    float x1, float y1,
                    float x2, float y2,
                    int thick);
float max(float x1, float x2);
float min(float x1, float x2);
float round(float x);

#endif // GRAPH_H
