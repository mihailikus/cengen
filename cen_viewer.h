#ifndef CEN_VIEWER_H
#define CEN_VIEWER_H

#include <QFile>
#include <QGraphicsScene>
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
    ~Cennic();

public slots:
    void setShablonName(const QDomNode& shablon);
    QPoint render(QGraphicsScene * scene, float X, float Y);
    QString money_format(float price, QString rub, QString kop);

private slots:
    QStringList mysplit(QString text);
    QStringList split_text(float maxlen, QString text, QFont font);
    void create(Tovar * tovar, const QDomNode& shablon);
    QString tra(QString text);
    void set_preview_mode(bool mode);
    bool preview_mode();

private:
    //QFile file;
    QDomNode domNode;
    QDomElement domElement;
    bool is_shablon_correct;
    bool preview;   //флаг для указания, что будет просто превью
    QString text;
    QRectF* rect;    //для указания области, в которой будет рисоваться элемент
    QTextCodec *codec;      //для перекодирования
    QPoint point;  //для возврата из ренрегинга правого нижнего угла

    Barcode* barcode;
    Tovar * ctovar;


};

#endif // CEN_VIEWER_H
