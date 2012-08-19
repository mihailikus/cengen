#ifndef IMAGE_H
#define IMAGE_H

#include <QGraphicsRectItem>
#include <QBrush>
#include <QFont>
#include <QToolBar>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

#include "models/abstract_item.h"

#include <QDebug>

QT_BEGIN_NAMESPACE
class QObject;
class QSpinBox;
class QLineEdit;
QT_END_NAMESPACE

class Image : public QObject, public QGraphicsRectItem, public AbstractItem {
    Q_OBJECT

public:
    Image(float w, float h, QString fileName);
    ~Image();

    int type() const;

    QToolBar* toolBar();


public slots:
    void on_sizeX_changed();
    void on_sizeY_changed();


    void on_fileButton_clicked();

signals:
    void some_changed();

};

#endif // IMAGE_H
