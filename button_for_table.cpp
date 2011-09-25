#include "button_for_table.h"

ButtonForTable::ButtonForTable(QWidget *parent) {
    position = 0;
    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));

}

void ButtonForTable::setPosition(int id) {
    position = id;
}

void ButtonForTable::onClicked() {
    emit clickedID(position);
}
