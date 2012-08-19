#ifndef BARCODEBOX_H
#define BARCODEBOX_H
#include <QGraphicsRectItem>
#include <QBrush>
#include <QFont>
#include <QToolBar>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFontDialog>

#include "models/abstract_item.h"

#include <QDebug>

QT_BEGIN_NAMESPACE
class QObject;
class QSpinBox;
class QLineEdit;
QT_END_NAMESPACE

class BarcodeBox : public QObject, public QGraphicsRectItem, public AbstractItem {
    Q_OBJECT

public:
    BarcodeBox(float w, float h);
    BarcodeBox(float startX, float startY, float w, float h);

    ~BarcodeBox();
    int type() const;

    QToolBar* toolBar();

    void setBrAddition(int addition);
    void setBrOtstup  (int otstup);


public slots:
    void on_size_changed();

    void on_braddition_changed() {
        brAddition = br_addition_box->value();
        emit some_changed();
    }
    void on_brotstup_changed() {
        brOtstup = br_otstup_box->value();
        emit some_changed();
    }
    void on_textline_changed() {
        itemText = itemTextLine->text();
        emit some_changed();
    }
    void on_fontSpin_chaned(int val) {
        itemFontSize = val;
        emit some_changed();
    }

    void on_fontButton_clicked();

    int addition() {return brAddition; }
    int otstup() {return brOtstup; }



private:
    QBrush getItemBrush(int tpt);
    int brAddition;
    int brOtstup;


    QSpinBox *br_addition_box, *br_otstup_box;  //эти для управления баркодом


signals:
    void some_changed();

};


#endif // BARCODEBOX_H
