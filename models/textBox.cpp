#include "models/textBox.h"


TextBox::TextBox (float w, float h, QString inputMethod) :
    QGraphicsRectItem(0, 0, w, h)
{
    method = inputMethod;

    itemToolBar = new QToolBar();
    setBrush(getItemBrush(ItemsTypes::textInBox));
    //qDebug() << this->brush();
    //brush();
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

TextBox::~TextBox() {}

void TextBox::on_size_changed() {
    qreal width = pwidthSpin->value() / this->sceneBoundingRect().width();
    qreal heith = pheithSpin->value() / this->sceneBoundingRect().height();
    //qDebug() << "w, h " << width << heith;
    scale(width, heith);
}

int TextBox::type() const {
    //qDebug() << "Type " << ItemType;
    return ItemsTypes::textInBox;
}

QToolBar * TextBox::toolBar() {

    makeToolBar(sceneBoundingRect().width(),
                sceneBoundingRect().height());

    connect(pheithSpin, SIGNAL(valueChanged(int)), SLOT(on_size_changed()));
    connect(pwidthSpin, SIGNAL(valueChanged(int)), SLOT(on_size_changed()));

    connect(itemTextLine, SIGNAL(textChanged(QString)), SLOT(on_textline_changed()));

    connect(fontSizeSpin, SIGNAL(valueChanged(int)), SLOT(on_fontSpin_chaned(int)));
    connect(fontButton, SIGNAL(clicked()), SLOT(on_fontButton_clicked()));


    return itemToolBar;
}

void TextBox::on_fontButton_clicked() {
    bool ok;

    QFont font = QFont(itemFont);
    font.setPointSize(itemFontSize);
    //qDebug() << "New italic " << font.italic();
    QFont fnt = QFontDialog::getFont(&ok, font);

    itemFont = fnt;
    fontSizeSpin->setValue(itemFont.pointSize());
    emit some_changed();
}
