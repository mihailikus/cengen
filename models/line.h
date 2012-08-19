#ifndef LINE_H
#define LINE_H

#include <QGraphicsRectItem>
#include <QBrush>
#include <QFont>
#include <QToolBar>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <math.h>

#include "models/abstract_item.h"

#include <QDebug>

QT_BEGIN_NAMESPACE
class QObject;
class QSpinBox;
class QLineEdit;
QT_END_NAMESPACE

class Line : public QObject, public QGraphicsRectItem, public AbstractItem {
    Q_OBJECT

public:
    Line(float len, float angl, float LineThick);
    ~Line();

    int type() const;

    QToolBar* toolBar();


public slots:
    void on_size_changed();

    float angl() { return itemAngl; }


    //void on_fileButton_clicked();

signals:
    void some_changed();

private:
    float itemLen, itemAngl, itemThick;

};
#endif // LINE_H
