#include <QFile>
#include <QDate>
#include <QGraphicsTextItem>
#include <QDebug>
#include "cen_viewer.h"
#include "barcode.h"

Cennic::Cennic(Tovar *tovar, const QDomNode &shablon) {
    barcode = new Barcode();
    this->create(tovar, shablon);
    this->preview = false;

}

Cennic::Cennic() {
    barcode = new Barcode();
    this->preview = false;
}

void Cennic::create(Tovar *tovar, const QDomNode &shablon) {
    barcode->update(tovar->barcode);

    this->codec = QTextCodec::codecForName("UTF-8");
    this->domNode = shablon;
    ctovar = tovar;
    if (ctovar->name_of_tovar == "") {
        ctovar->name_of_tovar = QObject::tr("tname is not set");
    } else {
        ctovar->name_of_tovar = codec->toUnicode(ctovar->name_of_tovar.toAscii());
    }
}

Cennic::~Cennic() {
    delete barcode;
}

QGraphicsItemGroup* Cennic::render(QGraphicsScene *scene, float X, float Y) {
    items = scene->createItemGroup(scene->selectedItems());
    QPen pen(Qt::black, 1);
    QBrush brush (Qt::white);
    point = QPoint(X, Y);
    QDomNode node = this->domNode.firstChild();
    while (!node.isNull()){
        //qDebug() << "rendering";
        if (node.isElement()) {
            domElement =  node.toElement();
            QString element = domElement.tagName();
            //qDebug() << "element " << element;

            //устанавливаем переменные из XML-файла
            //некоторые из них могут и не быть - пофиг, есть дефолтные значения
            QString method = domElement.attribute("method", "");
            float width =  domElement.attribute("width", "10").toFloat();
            float heith =  domElement.attribute("heith", "10").toFloat();
            float startX = domElement.attribute("startX", "0").toFloat();
            float startY = domElement.attribute("startY", "0").toFloat();
            int font_size = domElement.attribute("font-size", "10").toInt();
            bool font_bold = domElement.attribute("font-bold", "0").toInt();
            bool font_italic = domElement.attribute("font-italic", "0").toInt();

            float linethick = domElement.attribute("linethick", "3").toFloat();
            int br_lineAddition = domElement.attribute("addition", "25").toInt();
            int br_otstup = domElement.attribute("otstup", "10").toInt();

            QString font_family = domElement.attribute("font-family", "sans");
            QFont font = QFont(font_family, font_size);
            font.setBold(font_bold);
            font.setItalic(font_italic);

            QString rubSymbol = domElement.attribute("rub", "");
            QString kopSymbol = domElement.attribute("kop", "");

            if (element == "base" ) {
                items->addToGroup(scene->addRect(X, Y, width, heith,pen, brush));
                point.setX(width);
                point.setY(heith);
            }

            if (element == "line" ) {
                float len = sqrt(width*width + heith*heith);
                float angl;
                if (width) {
                    angl = atan(heith / width) * 180 / M_PI;
                } else {
                    angl = 90;
                }

                QGraphicsRectItem* rect1 = scene->addRect(0, 0, len, linethick, QPen(Qt::black), QBrush(Qt::black));
                rect1->setPos(X+startX, Y+startY);
                rect1->rotate(angl);
                items->addToGroup(rect1);
            }

            if (element == "barcode") {
                rect = new QRectF(X + startX, \
                                 Y + startY, \
                                 width, \
                                 heith);
                barcode->setFont(font);
                barcode->setLineAddition(br_lineAddition);
                barcode->setTextOtstup(br_otstup);
                items->addToGroup(barcode->render(scene, rect));
            }

            if (element == "good") {
                if (!preview) {
                    //печать названия товара - в несколько строк и по центру

                    QStringList lines =  this->split_text(width, ctovar->name_of_tovar, font, scene);

                    //рассчитываем высоту прямоугольника, чтобы расположить название
                    //вертикально по центру независимо от числа строк в названии
                    //int i=0;
                    float totalY=0;
                    int lines_count = lines.count();

                    /* if (lines_count>3) {
                        lines_count = 3;    //максимальное число строк в поле для названия товара
                    }
                    */
                    QList<float> Xtemp;
                    QList<float> Ytemp;

                    //сложить два цикла в один, ибо нефиг
                    for (int j=0; j<lines_count; j++) {
                        //располагаем первую строчку по центру
                        //qDebug() << "imagebox";
                        QRectF coord = imagebox(font, lines.at(j), scene);
                        //qDebug() << "imagebox fixed";
                        Xtemp << coord.width() / 2;
                        Ytemp << coord.height();
                        totalY += coord.height();
                    }

                    //qDebug() << "totalY = " << totalY;
                    while (totalY > heith && lines_count>1) {
                        //если текст по высоте выходит за рамки -
                        // то удаляем лишнюю строку нафиг
                        totalY -= Ytemp[--lines_count];
                    }

                    //верхний угол:
                    float corner = Y + startY + (heith - totalY) / 2;


                    for (int i = 0; i< lines_count; i++)
                    {
                            QGraphicsTextItem* item = scene->addText(lines.at(i), font);

                            float coordX = X + startX + width/2 - Xtemp.at(i);
                            //qDebug() << "coordX=" << coordX;
                            item->setPos(coordX, corner);
                            items->addToGroup(item);
                            corner += Ytemp.at(i);
                    }

                } else {
                    //если превью - рисуем просто прямоугольник
                    items->addToGroup(scene->addRect(X+startX, Y+startY, width, heith));
                }

            }

            if (element == "text") {
                if (!preview) {
                    if (method == "textInBox") {
                        //text = codec->toUnicode(domElement.text().toAscii());
                        text = domElement.text();


                    }
                    if (method == "date") {
                        QDate today = QDate::currentDate();
                        text = today.toString(domElement.text());
                    }
                    if (method == "nomer") {
                        //qDebug() << "tnomer" << ctovar->nomer_of_tovar;
                        text = QString::number(ctovar->nomer_of_tovar);

                    }
                    if (method == "price") {
                        text = this->money_format(ctovar->price1, rubSymbol, kopSymbol);
                        //qDebug() << "Price=" << text;
                    }
                    if (method == "priceOld") {
                        text = this->money_format(ctovar->price2, rubSymbol, kopSymbol);
                    }
                    if (method == "barcodeAsText") {
                        text = ctovar->barcode;
                    }
                    if (method == "quantity") {
                        text = QString::number(ctovar->quantity);
                    }

                    //qDebug() << "method = " << method;
                    QGraphicsTextItem* textItem = scene->addText(text, font);
                    QRectF textRect = textItem->boundingRect();
                    float newX = X +  startX + \
                                 (width - textRect.width()) / 2;
                    float newY = Y + startY + \
                                 (heith - textRect.height()) / 2;
                    textItem->setPos(newX, newY);
                    items->addToGroup(textItem);
                } else {
                    //если превью - рисуем просто прямоугольник
                    items->addToGroup(scene->addRect(X+startX, Y+startY, width, heith));
                }


            }

            if (element == "image" ) {
                QPixmap pm = QPixmap(domElement.text());
                QGraphicsPixmapItem *px = scene->addPixmap(pm);
                px->setPos(X+startX, Y+startY);
                float sc = width / pm.width();
                px->scale(sc, sc);
                items->addToGroup(px);
            }

        }

        node = node.nextSibling();
    }
return (items);
}

QStringList Cennic::split_text(float maxlen, QString text, QFont font, QGraphicsScene *scene) {
    QStringList lines = mysplit(text);
    QStringList sep_lines;

    QString line;
    QString line_old = line;
    float text_l;
    int i=1;
    int j = lines.count(); // count($lines);
    if (j == 1) return lines; //lines[0];
    //echo "text_l = ". $text_l . "; maxlen = ". $maxlen ."<br>";
    float text_old_l;
    while (i < j) {
        line = lines.at(i-1);
        text_l = 0;
        while ( (text_l < maxlen) && (i < j)  ) {

            text_old_l = text_l;
            line_old = line;
            line += lines.at(i);

            text_l = imagebox(font, line, scene).width();
            i++;
        }
        if (text_l <= maxlen) {
            line_old += lines.at(i-1);
            i++;
        }
        sep_lines << line_old;
    }
    if (i == j) {
        sep_lines << lines.at(j-1);
    }
    return sep_lines;
}

QString Cennic::money_format(float price, QString rubl, QString kope) {
    QString money =  QString("%1").arg(price,0,'f',2);
    QStringList money_lines = money.split(".");
    QString money_format;
    money_format = "";
    if (rubl != "") {
        money_format += money_lines.at(0) + rubl;
    }
    if (kope != "") {
        money_format += money_lines.at(1) + kope;
    }
    return money_format;
}

void Cennic::set_preview_mode(bool mode) {
    this->preview = mode;
}

bool Cennic::preview_mode() {
    qDebug() << "view mode is " << preview;
    return preview;
}

QPoint Cennic::lastCorner() {
    return point;
}
