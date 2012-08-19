#include "models/barcodeBox.h"

BarcodeBox::BarcodeBox(float w, float h) :
    QGraphicsRectItem(0, 0, w, h)
  //::QWidget()
{
    itemToolBar = new QToolBar();
    //setBrush(getItemBrush(ItemsTypes::Barcode));
    setBrush(QBrush(Qt::darkCyan));
    brush();
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);


}
BarcodeBox::BarcodeBox(float startX, float startY, float w, float h) :
    QGraphicsRectItem(startX, startY, w, h)
{
    itemToolBar = new QToolBar();
    //setBrush(getItemBrush(ItemsTypes::Barcode));
    //brush();

}

BarcodeBox::~BarcodeBox() {

}

void BarcodeBox::setBrAddition(int addition) {
    brAddition = addition;
}

void BarcodeBox::setBrOtstup(int otstup) {
    brOtstup = otstup;
}

int BarcodeBox::type() const {
    return ItemsTypes::Barcode;
}

QToolBar * BarcodeBox::toolBar() {

    makeToolBar(sceneBoundingRect().width(),
                sceneBoundingRect().height());

    br_addition_box = new QSpinBox;
    br_addition_box->setMinimum(-100);
    br_addition_box->setMaximum(100);

    br_otstup_box = new QSpinBox;
    br_otstup_box->setMinimum(-100);
    br_otstup_box->setMaximum(100);

    itemToolBar->addWidget(br_addition_box);
    itemToolBar->addWidget(br_otstup_box);


    br_addition_box->setValue(brAddition);
    br_otstup_box->setValue(brOtstup);


    connect(pheithSpin, SIGNAL(valueChanged(int)), SLOT(on_size_changed()));
    connect(pwidthSpin, SIGNAL(valueChanged(int)), SLOT(on_size_changed()));

    connect(br_otstup_box, SIGNAL(valueChanged(int)), SLOT(on_brotstup_changed()));
    connect(br_addition_box, SIGNAL(valueChanged(int)), SLOT(on_braddition_changed()));
    connect(itemTextLine, SIGNAL(textChanged(QString)), SLOT(on_textline_changed()));

    connect(fontSizeSpin, SIGNAL(valueChanged(int)), SLOT(on_fontSpin_chaned(int)));
    connect(fontButton, SIGNAL(clicked()), SLOT(on_fontButton_clicked()));


    return itemToolBar;
}



void BarcodeBox::on_size_changed() {
    qreal width = pwidthSpin->value() / this->sceneBoundingRect().width();
    qreal heith = pheithSpin->value() / this->sceneBoundingRect().height();
    //qDebug() << "w, h " << width << heith;
    scale(width, heith);
    //resize(pwidthSpin->value(), pheithSpin->value());
    //emit some_changed();
}

void BarcodeBox::on_fontButton_clicked() {
    bool ok;

    QFont font = QFont(itemFont);
    font.setPointSize(itemFontSize);
    //qDebug() << "New italic " << font.italic();
    QFont fnt = QFontDialog::getFont(&ok, font);

    itemFont = fnt;
    fontSizeSpin->setValue(itemFont.pointSize());
    emit some_changed();
}
