#ifndef ABSTRACT_ITEM_H
#define ABSTRACT_ITEM_H

#include <QString>
#include <QFont>
#include <QToolBar>
#include <QSpinBox>
#include <QPushButton>
#include <QLineEdit>


class AbstractItem {


public:
    struct ItemsTypes
    {
        static int const Barcode   = 700;
        static int const textInBox = 701;
        static int const Good      = 702;
        static int const Image     = 703;
        static int const Line      = 704;

    };

    int ItemType;



    QString itemText;
    QFont itemFont;
    int itemFontSize;
    QToolBar *itemToolBar;
    QSpinBox *fontSizeSpin;
    QSpinBox *pwidthSpin, *pheithSpin;
    QLineEdit *itemTextLine;
    QPushButton *fontButton;


    QString method;

    void makeToolBar(int w, int h, QString buttonText = "FONT");


    void setText(QString text);
    int fontSize() { return itemFontSize; }
    QString fontFamily() { return itemFont.family(); }
    bool fontBold() {return itemFont.bold(); }
    bool fontItalic() {return itemFont.italic(); }
    QString text() {return itemText; }

    void setFont(int fontSize, QString fontFamily, bool Bold, bool Italic);
    QBrush getItemBrush(int tpt);
    QString getMethod() {return method; }

private:





};


#endif // ABSTRACT_ITEM_H
