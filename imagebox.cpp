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

QStringList ImageBox::mysplit(QString text) {
    QString pattern = "[-/;: ,+(.)\"]";
    QStringList arr = text.split(QRegExp(pattern));
    int j = arr.count() - 1;
    QString *lines = new QString[arr.count()];
    for (int i = 0; i<=j; i++) {
        lines[i] = arr.at(i);
    }

    //признак парности скобок - 0 нет, 1 была
    //  (вложенные не учитываем, вроде не надо)
    bool skob_chek = 0;
    int i = 0;
    int position = 0;

    QStringList goodlines;
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
        i++;
    }

    for (int i = 0; i<=j; i++) {
        goodlines << lines[i];
    }

    return goodlines;
}

QString ImageBox::superSplit(QString text) {
    QStringList lst = mysplit(text);
    QString tmp = "";
    for (int i = 0; i<lst.count(); i++) {
        tmp += lst.at(i) + " ";
    }
    return tmp;
}
