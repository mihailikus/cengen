#ifndef BUTTON_FOR_TABLE_H
#define BUTTON_FOR_TABLE_H
#include <QPushButton>

class ButtonForTable : public QPushButton {
    Q_OBJECT

public:
    ButtonForTable(QWidget *parent = 0);
    int position;
signals:
    void clickedID(int position);
public slots:
    void setPosition(int position);
private slots:
    void onClicked();


};

#endif // BUTTON_FOR_TABLE_H
