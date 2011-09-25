#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QTableWidget>


QT_BEGIN_NAMESPACE
class QGridLayout;
class QTableWidget;
QT_END_NAMESPACE

//class QLineEdit;

//=================

class MyDialog : public QDialog {
    Q_OBJECT
private:
    QGridLayout* myLayout;
    QTableWidget* myTable;
    QPushButton* buttonOK;
    QPushButton* buttonCancel;
    //QScrollArea* area;


public:
    MyDialog (QWidget* pwgt = 0);

    //QString dialogText() const;
    void setTable(QTableWidget **table);
    void setTable(QString error);

private slots:
    void addTovarToList(int row, int column);

};

#endif // DIALOG_H
