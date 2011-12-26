#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QTableWidget>


QT_BEGIN_NAMESPACE
class QGridLayout;
class QTableWidget;
QT_END_NAMESPACE

class MyDialog : public QDialog {
    Q_OBJECT
private:
    QGridLayout* myLayout;
    QTableWidget* myTable;
    QPushButton* buttonOK;
    QPushButton* buttonCancel;
    QPushButton* buttonSelectAll;

    bool itemsToSelectAll;

public:
    MyDialog (QWidget* pwgt = 0);

    void setTable(QTableWidget **table);
    void setTable(QString error);

private slots:
    void addTovarToList(int row, int column);
    void selectAllItems();

};

#endif // DIALOG_H
