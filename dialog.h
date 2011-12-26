#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QTableWidget>


QT_BEGIN_NAMESPACE
class QGridLayout;
class QTableWidget;
QT_END_NAMESPACE

class ListFoundedItemsDialog : public QDialog {
    Q_OBJECT
private:
    QGridLayout* myLayout;
    QTableWidget* myTable;
    QPushButton* buttonOK;
    QPushButton* buttonCancel;
    QPushButton* buttonSelectAll;

    bool itemsToSelectAll;

public:
    ListFoundedItemsDialog (QWidget* pwgt = 0);

    void setTable(QTableWidget **table);
    void setTable(QString error);

private slots:
    void addTovarToList(int row, int column);
    void selectAllItems();

};

#endif // DIALOG_H
