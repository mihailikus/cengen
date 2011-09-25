#include "addlinemaster.h"
#include "ui_addlinemaster.h"

#include <QDebug>

AddLineMaster::AddLineMaster(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddLineMaster)
{
    ui->setupUi(this);

    buttonOK = qFindChild<QPushButton*>(this, "OK_but");
    buttonCancel = qFindChild<QPushButton*>(this, "can_but");

    connect(buttonOK, SIGNAL(clicked()), SLOT(accept()));
    connect(buttonCancel, SIGNAL(clicked()), SLOT(reject()));

    ui_startXspin = qFindChild<QSpinBox*>(this, "startXspin");
    ui_startYspin = qFindChild<QSpinBox*>(this, "startYspin");
    ui_widthSpin = qFindChild<QSpinBox*>(this, "widthSpin");
    ui_heithSpin = qFindChild<QSpinBox*>(this, "heithSpin");
    ui_thickSpin = qFindChild<QSpinBox*>(this, "thickSpin");

    connect (ui_startXspin, SIGNAL(valueChanged(int)), SLOT(on_values_changed()));
    connect (ui_startYspin, SIGNAL(valueChanged(int)), SLOT(on_values_changed()));
    connect (ui_widthSpin, SIGNAL(valueChanged(int)), SLOT(on_values_changed()));
    connect (ui_heithSpin, SIGNAL(valueChanged(int)), SLOT(on_values_changed()));
    connect (ui_thickSpin, SIGNAL(valueChanged(int)), SLOT(on_values_changed()));


    line_number = 0;



}

AddLineMaster::~AddLineMaster()
{
    delete ui;
}


void AddLineMaster::on_values_changed() {

    qDebug() << "values changed";

    float startX, startY, width, heith, thick;
    startX = ui_startXspin->value();
    startY = ui_startYspin->value();
    width = ui_widthSpin->value();
    heith = ui_heithSpin->value();
    thick = ui_thickSpin->value();

    emit values_changed("line" + QString::number(line_number), startX, startY, width, heith, thick);
}

void AddLineMaster::set_line_number(int number) {
    qDebug() << "LINE number setted " << number;
    this->line_number = number;
}

void AddLineMaster::set_values(float startX, float startY, float width, float heith, float thick) {
    ui_startXspin->blockSignals(true);
    ui_startYspin->blockSignals(true);
    ui_widthSpin->blockSignals(true);
    ui_heithSpin->blockSignals(true);
    ui_thickSpin->blockSignals(true);

    ui_startXspin->setValue(startX);
    ui_startYspin->setValue(startY);
    ui_widthSpin->setValue(width);
    ui_heithSpin->setValue(heith);
    ui_thickSpin->setValue(thick);

    ui_startXspin->blockSignals(false);
    ui_startYspin->blockSignals(false);
    ui_widthSpin->blockSignals(false);
    ui_heithSpin->blockSignals(false);
    ui_thickSpin->blockSignals(false);

}
