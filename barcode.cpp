#include <QtGui>
#include <QDebug>
#include "barcode.h"


Barcode::Barcode(QString text)
{
    //qDebug() <<"barcode SUB creation: " << text;
    this->setText(text);
    if (this->is_valid()) {
        this->TextToBit();
        //qDebug() << "barcode VALID";
    } else {
        //qDebug() << "barcode WRONG";
        for (int i=0; i<95; i++)
        {
            lines << false;
        }
    }
    this->renderNumbers = true;
    this->font = QFont("arial", 5);
    this->textOtstup = 0.0;
    this->lineAddition = 10.0;

}

Barcode::~Barcode()
{

}

bool Barcode::is_valid()
{
    if (barcode.length()<6) return false;
    int sum = 0;
    int k = 1;
    for (int i = barcode.length()-1; i>=0; i--)
    {
        sum += k*(barcode[i].toAscii()-48);
        if (k==1) {
            k=3;
          } else {
            k=1;
          }
    }
    int ksum = sum % 10;
    if (!ksum) return true;
    return false;
}

bool Barcode::setText(QString text)
{
    barcode = "";
    for (int i=text.length(); i<13; i++)
    {
        barcode+="0";
    }
    barcode += text;
    TextToBit();
    return is_valid();
}

QList<bool> Barcode::TextToBit()
{
    QList<QString> bitlist;
    QString shtrih = "";

    //битовые коды для стиля UPC-A
    bitlist << "0001101"   //0
            << "0011001"    //1
            << "0010011"    //2
            << "0111101"    //3
            << "0100011"    //4
            << "0110001"    //5
            << "0101111"    //6
            << "0111011"    //7
            << "0110111"
            << "0001011";   //9

    QList<QString>  masklist;
    //masklist используется для задания кода в стиле EAN-13
    masklist << "LLLLLL"
             <<	"LLGLGG"
             <<	"LLGGLG"
             <<	"LLGGGL"
             << "LGLLGG"    //4
             <<	"LGGLLG"
             << "LGGGLL"
             <<	"LGLGLG"
             <<	"LGLGGL"
             <<	"LGGLGL";

    int firstSymbol = correctSymbol(barcode[0]);
            //barcode[0].toAscii()-48;  //первый символ - он сам не имеет штрихов

    QString mask = masklist.at(firstSymbol);

    shtrih += "101";

    QChar L = masklist.at(0)[0];
    QChar symbol;
    QString add;
    int bit;

    for (int i = 1; i<7; i++)   //кодируем первую половину кода
    {
        symbol = barcode[i];
        bit = correctSymbol(barcode[i]);
                //symbol.toAscii()-48;

        //qDebug() << bit;
        if (mask[i-1] == L)
        {
            //если для кодирования знака используется прямой L-код
            add = bitlist.at(bit);

        } else {
            //иначе - переворачиваем bitlist и делаем негативным
            add = GCode(bitlist.at(bit));
        }
        shtrih += add;


    }
    shtrih += "01010";

    for (int i = 7; i<13; i++)
    {
        int bit = correctSymbol(barcode[i]);
        add = RCode(bitlist.at(bit));
        shtrih += add;
    }

    shtrih += "101";

    lines.clear();

    for (int i=0; i<95; i++)
    {
        if (shtrih[i].toAscii()-48) {
            lines << true;
        } else {
            lines << false;
        }
    }
return lines;
}

QString Barcode::RCode(QString bitlist)
{
    QString ret = "";
    for (int i=0; i<=6; i++)
    {
        int bit = bitlist[i].toAscii()-48;
        if (bit) {
            ret += "0";
        } else {
            ret += "1";
        }
    }
    return ret;
}

QString Barcode::GCode(QString bitlist)
{
    QString reverse = "";
    for (int i=6; i>=0; i--)
    {
        int bit = bitlist[i].toAscii()-48;
        if (bit) {
            reverse += "0";
        } else {
            reverse += "1";
        }
    }
    return reverse;
}

void Barcode::render(QGraphicsScene *scene, QRectF* inputRect)
{
    //функция рендеринга EAN-13

    //scene->addRect(rect->x(), rect->y(), rect->width(), rect->height());

    //номера системных штрихов - они печатаются удлиненными
    QList<int> systemLines;
    systemLines << 0 << 1<<  2 << 45 << 46 << 47 << 48 << 49 << 94 << 93 << 92;

    float left_otstup = imagebox(font, this->barcode.at(0)).width();

    QRectF* rect = new QRectF(inputRect->left()+left_otstup,
                            inputRect->top(),
                            inputRect->width()-left_otstup,
                            inputRect->height() - lineAddition*2 - textOtstup);

    float line_width = (rect->width()) / 95;    //количство линий в штрихкоде EAN-13
    //qDebug() << "width = " << line_width;
    float line_heit = rect->height();

    float Xposition = rect->x();
    float Yposition = rect->y();
    QPen pen(Qt::black, 1);
    QBrush brush (Qt::green);
    int count = 0;
    float heit;
    for (int i = 0; i<95; i++)
    {
        if (lines.at(i))
        {
            //если для штриха установлен темный код
            pen.setColor(Qt::black);
            brush.setColor(Qt::black);

        } else {
            //если для штриха установлен светлый код
            pen.setColor(Qt::white);
            brush.setColor(Qt::white);
        }

        if ( systemLines.contains(count))
        {
            heit = line_heit+this->lineAddition + textOtstup;
        } else {
            heit = line_heit;
        }

        //qDebug() << "rendering barcode";
        scene->addRect(QRect(Xposition, Yposition, line_width-1, heit), pen, brush);
        Xposition += line_width;
        //qDebug() << "Position=" << position << "; width=" << line_width;

        count++;
    }
    if (this->renderNumbers)
    {
        count = 0;
        Xposition = inputRect->left();
        Yposition = inputRect->y() + inputRect->height() - lineAddition*2;
        QString text = "0";
        //qDebug() << "this FONT" << this->font;
        for (int i = 0; i<13; i++)
        {
            text[0] = barcode[i].toAscii();
            QGraphicsTextItem * item = scene->addText(text, font);
            item->setPos(Xposition, Yposition);
            Xposition += 7*line_width;
            if ( (i==6) || (i==0)) {
                Xposition += line_width*4;
            }

        }
    }


}

QString Barcode::getText()
{
    return barcode;
}

void Barcode::setFont(QFont inputfont)
{
    this->font = inputfont;
    if (font.pixelSize() == 0) {
        this->renderNumbers = false;
        //this->lineAddition = 0;
    }

    lineAddition = imagebox(font, this->getText()).height() / 2;

}

void Barcode::setTextOtstup(int otstup)
{
    this->textOtstup = otstup;
    qDebug() << "using function setTextOtstup " << otstup;
}

void Barcode::setLineAddition(float addition)
{
    this->lineAddition = addition;
    qDebug() << "using function setLineAddition " << addition;
}

void Barcode::setRenderDigits(bool param) {
    this->renderNumbers = param;
}

int Barcode::correctSymbol(QChar symbol) {
    int bit = symbol.toAscii() - 48;
    if ( (bit>9) || (bit < 0) ) {
        //неправильный символ - исправляем на нуль
        bit = 0;
    }
    return bit;
}
