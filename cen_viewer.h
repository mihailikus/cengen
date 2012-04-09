#ifndef CEN_VIEWER_H
#define CEN_VIEWER_H

#include <QFile>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QTextCodec>
#include <QString>
#include <QtXml>
#include "barcode.h"
#include "tovar.h"
#include "graph.h"
#include "imagebox.h"

class Cennic : public ImageBox {
public:

    Cennic(Tovar * tovar, const QDomNode& shablon);
    Cennic();
    ~Cennic();

public slots:
    void create(Tovar * tovar, const QDomNode& shablon);
    QGraphicsItemGroup *render(QGraphicsScene * scene, float X, float Y);
    QString money_format(float price, QString rub, QString kop);
    QPoint lastCorner();

private slots:
    QStringList mysplit(QString text);
    QStringList split_text(float maxlen, QString text, QFont font);
    void set_preview_mode(bool mode);
    bool preview_mode();

private:
    //QFile file;
    QDomNode domNode;
    QDomElement domElement;
    bool preview;   //флаг для указания, что будет просто превью
    QString text;
    QRectF* rect;    //для указания области, в которой будет рисоваться элемент
    QTextCodec *codec;      //для перекодирования

    Barcode* barcode;
    Tovar * ctovar;

    QGraphicsItemGroup *items;

    QPoint point;

};

#endif // CEN_VIEWER_H
