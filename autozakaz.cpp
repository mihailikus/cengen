#include "autozakaz.h"
#include "ui_autozakaz.h"

autozakaz::autozakaz(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::autozakaz)
{
    ui->setupUi(this);
    line1 = qFindChild<QLineEdit*>(this, "lineEdit");
    line2 = qFindChild<QLineEdit*>(this, "lineEdit_2");
    line3 = qFindChild<QLineEdit*>(this, "lineEdit_3");
    line4 = qFindChild<QLineEdit*>(this, "lineEdit_4");
    line5 = qFindChild<QLineEdit*>(this, "lineEdit_5");
    cal1 = qFindChild<QCalendarWidget*>(this, "calendarStart");
    cal2 = qFindChild<QCalendarWidget*>(this, "calendarStop");

    cal1->setSelectedDate(QDate::currentDate().addDays(-4));
    cal2->setSelectedDate(QDate::currentDate().addDays(4));



}

void autozakaz::set_config(autozakaz_config cfg) {
    cal1->setSelectedDate(cfg.dateStart);
    cal2->setSelectedDate(cfg.dateStop);
    line1->setText(cfg.ostat_magazin);
    line2->setText(cfg.ostat_sklad);
    line3->setText(cfg.assort);
    line4->setText(cfg.kol_v_korob);
    line5->setText(cfg.korob_quantum);

}

autozakaz_config autozakaz::get_config() {
    autozakaz_config cfg;
    cfg.dateStart = cal1->selectedDate();
    cfg.dateStop = cal2->selectedDate();
    cfg.ostat_magazin = line1->text();
    cfg.ostat_sklad = line2->text();
    cfg.assort = line3->text();
    cfg.kol_v_korob = line4->text();
    cfg.korob_quantum = line5->text();
    return cfg;
}

autozakaz::~autozakaz()
{
    delete ui;
}

QString autozakaz::get_filter_file_name(QLineEdit *line) {
    QString file = line->text();
    QString txt = "";
    QFileInfo fi(file);
    if (fi.exists()) {
        txt = file;
    } else {
        txt = QApplication::applicationDirPath();
    }
    QString fopen =  QFileDialog::getOpenFileName(this, tr("Open filter settings"), txt, tr("Filter settings (*.fli)"));

    if (fopen == "") {
        return line->text();
    }
    return fopen;

}

void autozakaz::on_pushButton_clicked()
{
    //выбор файла настроек фильтра остатков магазина
    QString fileName = get_filter_file_name(line1);
    line1->setText(fileName);

}

void autozakaz::on_pushButton_2_clicked()
{
    //выбор файла настроек фильтра остатков товара на складе
    QString fileName = get_filter_file_name(line2);
    line2->setText(fileName);
}

void autozakaz::on_pushButton_3_clicked()
{
    //выбор файла настроек фильтра обязательного ассортимента
    QString fileName = get_filter_file_name(line3);
    line3->setText(fileName);
}

void autozakaz::on_pushButton_4_clicked()
{
    //выбор файла настроек фильтра количества товара в коробках
    QString fileName = get_filter_file_name(line4);
    line4->setText(fileName);

}

void autozakaz::on_pushButton_5_clicked()
{
    //выбор файла настроек фильтра квантования коробок
    QString fileName = get_filter_file_name(line5);
    line5->setText(fileName);

}

int autozakaz::get_days_for_zakaz() {
    QDate dt2 = cal2->selectedDate();
    int days = QDate::currentDate().daysTo(dt2);
    return days;
}
