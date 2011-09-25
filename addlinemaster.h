#ifndef ADDLINEMASTER_H
#define ADDLINEMASTER_H

#include <QDialog>
#include <QPushButton>
#include <QSpinBox>
#include <QGraphicsView>

QT_BEGIN_NAMESPACE
class QPushButton;
class QGraphicsView;
class QSpinBox;
QT_END_NAMESPACE

namespace Ui {
    class AddLineMaster;
}

class AddLineMaster : public QDialog
{
    Q_OBJECT

public:
    explicit AddLineMaster(QWidget *parent = 0);
    ~AddLineMaster();

    void update_preview();
    void set_line_number(int number);
    void set_values(float startX, float startY, float width, float heith, float thick);

private:
    Ui::AddLineMaster *ui;
    QPushButton *buttonOK, *buttonCancel;
    QSpinBox *ui_startXspin, *ui_startYspin, *ui_widthSpin, *ui_heithSpin, *ui_thickSpin;
    QGraphicsView *ui_grView;
    int line_number;

signals:
    void values_changed(QString type, float startX, float startY, float width, float heith, float thick);

private slots:
    void on_values_changed();





};

#endif // ADDLINEMASTER_H
