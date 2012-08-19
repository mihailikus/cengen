#ifndef GOOD_H
#define GOOD_H

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

class Good : public QObject, public QGraphicsRectItem, public AbstractItem {
    Q_OBJECT

public:
    Good(float w, float h);
    ~Good();

    int type() const;

    QToolBar* toolBar();


public slots:
    void on_size_changed();
    void on_textline_changed() {
        itemText = itemTextLine->text();
        emit some_changed();
    }
    void on_fontSpin_chaned(int val) {
        itemFontSize = val;
        emit some_changed();
    }


    void on_fontButton_clicked();

signals:
    void some_changed();

};



#endif // GOOD_H
