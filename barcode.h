#ifndef BARCODE_H
#define BARCODE_H
#include <QGraphicsScene>
#include <QList>
#include <QFont>
#include "imagebox.h"


class Barcode : public ImageBox {

private:
    QList<bool> lines;  //95 штрихов
    QString barcode;    //содержит текстовую строку-штрихкод
    //int font;       //штрифт для нанесения цифр внизу штрихкода
    QFont font;
    bool renderNumbers; //если 1 - будут рисоваться цифры внизу штрих-кода
    float textOtstup; //отступ цифр от линий штрих-кода
    float lineAddition; //удлинение линий служебных штрихов
    QGraphicsItemGroup *items;   //собираем все элементы штрих-кода в группу

public:
    Barcode(QString text = "0000000000000");
    ~Barcode();
    QString found_lost_digit(QString text);

public slots:
    void update(QString text = "0000000000000");
    bool is_valid();
    bool setText(QString barcode);
    QList<bool> TextToBit();
    QGraphicsItemGroup *render (QGraphicsScene * scene, QRectF* rect);
    QString getText();
    void setFont(QFont font);
    void setTextOtstup (int otstup);
    void setLineAddition (int addition);
    void setRenderDigits(bool param);



private slots:
    QString GCode(QString text);
    QString RCode(QString text);
    int correctSymbol(QChar symbol);

};

#endif // BARCODE_H
