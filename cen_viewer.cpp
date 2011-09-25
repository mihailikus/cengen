#include <QFile>
#include <QDate>
#include <QGraphicsTextItem>
#include <QDebug>
#include "cen_viewer.h"
#include "barcode.h"

Cennic::Cennic(Tovar *tovar, const QDomNode &shablon) {

    this->create(tovar, shablon);
    this->preview = false;

}

void Cennic::create(Tovar *tovar, const QDomNode &shablon) {
    //qDebug() << "cennic SUB creation";
    barcode = new Barcode(tovar->barcode);
    //qDebug() <<barcode->is_valid();

    this->codec = QTextCodec::codecForName("UTF-8");
    on_page_is_set = false;
    this->setShablonName(shablon);
    ctovar = tovar;
    if (ctovar->name_of_tovar == "") {
        ctovar->name_of_tovar = "tname is not set";
    } else {
        ctovar->name_of_tovar = codec->toUnicode(ctovar->name_of_tovar.toAscii());
    }
}

void Cennic::setShablonName(const QDomNode& shablon) {
    this->domNode = shablon;
    if (!domNode.isNull()) {
        is_shablon_correct = true;
    } else {
        is_shablon_correct = false;
    }
}

Cennic::~Cennic() {
    qDebug() << "destructor of cennic";
}

QPoint* Cennic::render(QGraphicsScene *scene, float X, float Y) {
    QPen pen(Qt::black, 1);
    QBrush brush (Qt::white);
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
            float linethick = domElement.attribute("linethick", "3").toFloat();
            QString font_family = domElement.attribute("font-family", "sans");
            QFont font = QFont(font_family, font_size);
            QString rubSymbol = domElement.attribute("rub", "");
            QString kopSymbol = domElement.attribute("kop", "");



            if (element == "base" ) {
                scene->addRect(X, Y, width, heith,pen, brush);
                point = new QPoint (width, heith);
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
            }

            if (element == "barcode") {
                rect = new QRectF(X + startX, \
                                 Y + startY, \
                                 width, \
                                 heith);
                barcode->setFont(font);
                barcode->render(scene, rect);
            }

            if (element == "good") {
                if (!preview) {
                    //печать названия товара - в несколько строк и по центру

                    QStringList lines =  this->split_text(width, ctovar->name_of_tovar, font);

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
                        QRectF coord = imagebox(font, lines.at(j));
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
                            corner += Ytemp.at(i);
                    }

                } else {
                    //если превью - рисуем просто прямоугольник
                    scene->addRect(X+startX, Y+startY, width, heith);
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

                    //qDebug() << "method = " << method;
                    QGraphicsTextItem* textItem = scene->addText(text, font);
                    QRectF textRect = textItem->boundingRect();
                    float newX = X +  startX + \
                                 (width - textRect.width()) / 2;
                    float newY = Y + startY + \
                                 (heith - textRect.height()) / 2;
                    textItem->setPos(newX, newY);
                } else {
                    //если превью - рисуем просто прямоугольник
                    scene->addRect(X+startX, Y+startY, width, heith);
                }


            }


        }

        node = node.nextSibling();
    }
return (point);
}

QStringList Cennic::mysplit(QString text) {
    QString pattern = "[-/;: ,+(.)\"]";
    QStringList arr = text.split(QRegExp(pattern));
    int j = arr.count() - 1;
    QString *lines = new QString[arr.count()];
    for (int i = 0; i<=j; i++) {
        lines[i] = arr.at(i);
        //qDebug()<< "lineYYY" << lines[i];
    }

    //признак парности скобок - 0 нет, 1 была
    //  (вложенные не учитываем, вроде не надо)
    bool skob_chek = 0;
    int i = 0;
    int position = 0;

    QList<QString> goodlines;
    goodlines.clear();

    while (i<j) {
        position += lines[i].length() + 1; //strlen($lines[$i]) + 1;
        QChar line1 = text[position-1];
        //$line1 = substr($text, $position-1, 1);

        if  (line1 == '\"' || line1 == '(' || line1 == ')') {
            if   (skob_chek)
                  {
                      lines[i] += line1;
                      skob_chek = false;
                  }
                else {
                    //если скобки еще не было - значит, символ относится к следующему слову
                      lines[i+1] = line1 + lines[i+1];
                      position--;
                      skob_chek = true;
                      }
            } else {
                if (line1 != '\n') {
                    //если это не парный символ и не символ переноса строки
                    //  то просто добавляем его
                    lines[i] += line1;
                }
        }
/*
        if (lines[i].length() == 1) {
            //если все слово - только из разделителя,
            //  то плюсуем разделитель к предыдущему слову
            lines[i-1] += lines[i];
            i++;
            position++;
        }
*/

        i++;
    }

    for (int i = 0; i<=j; i++) {
        goodlines << lines[i];
    }

    //qDebug() << "Updated lines: " << goodlines;
return goodlines;
}

QStringList Cennic::split_text(float maxlen, QString text, QFont font) {
    QStringList lines = this->mysplit(text);
    QStringList sep_lines;
    //scene->addText()
    //qDebug() << "maxlen = " << maxlen;


    //qDebug() << "split text func: " << lines;
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
        //qDebug() << "line=" << line;
        text_l = 0;
        while ( (text_l < maxlen) && (i < j)  ) {

            text_old_l = text_l;
            line_old = line;
            //qDebug() << lines.at(i);
            line += lines.at(i);

            //qDebug() << "imagebox again";
            QRectF coord1 = imagebox(font, line);
            //qDebug() << "imagebox again2";
            text_l = coord1.width();
            //qDebug() << "maxlen=" << maxlen << "; " << i<< "; text length = " << text_l;
            i++;
            //echo "text_l = ". $text_l . "; maxlen = ". $maxlen ."; i=". $i . "<br>";
        }
        if (text_l <= maxlen) {
            line_old += lines.at(j-1);
            i++;
        }

        //qDebug() << "Line old = " << line_old << ", width = " << text_old_l;
        sep_lines << line_old;

    }
    if (i == j) {
        sep_lines << lines.at(j-1);
    }
    //qDebug() << sep_lines;
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

QString Cennic::tra(QString text) {
    return codec->toUnicode(text.toAscii());
}

void Cennic::set_preview_mode(bool mode) {
    this->preview = mode;
}

bool Cennic::preview_mode() {
    qDebug() << "view mode is " << preview;
    return preview;

}
