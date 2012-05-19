#ifndef AUTOZAKAZ_H
#define AUTOZAKAZ_H

#include <QWizard>
#include <QLineEdit>
#include <QFileDialog>
#include <QCalendarWidget>
//#include "file_worker.h"

namespace Ui {
class autozakaz;
}

struct autozakaz_config {
    QDate dateStart;
    QDate dateStop;
    QString ostat_magazin;
    QString ostat_sklad;
    QString assort;
    QString kol_v_korob;
    QString korob_quantum;
};

class autozakaz : public QWizard
{
    Q_OBJECT
    
public:
    explicit autozakaz(QWidget *parent = 0);
    ~autozakaz();
    void set_config(autozakaz_config cfg);
    int get_days_for_zakaz();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    QString get_filter_file_name(QLineEdit *line);

private:
    Ui::autozakaz *ui;
    QLineEdit* line1, *line2, *line3, *line4, *line5;
    QCalendarWidget *cal1, *cal2;
};

#endif // AUTOZAKAZ_H
