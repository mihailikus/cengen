#ifndef SELLFILTERWIDGET_H
#define SELLFILTERWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QCalendarWidget>
#include <QSettings>
#include <QToolBar>
#include <QFileInfo>
#include <QFileDialog>
#include <QProgressBar>
#include <QStatusBar>
#include <QApplication>
#include <QDebug>
#include "tinformer.h"
#include "dialog.h"
//#include "cengen.h"

QT_BEGIN_NAMESPACE
class QGridLayout;
class QLabel;
class QComboBox;
class QPushButton;
class QCalendarWidget;
class QSettings;
class QToolBar;
QT_END_NAMESPACE


class SellFilterWidget : public QWidget {
    Q_OBJECT

public:
    SellFilterWidget(QToolBar *ui_mainToolBar,
                     QStatusBar *statBar,
                     QWidget* pwgt = 0);
    ~SellFilterWidget();

    void check_sell_file();
    bool is_sell_file_good();

    QList<Tovar> apply_sell_filter(QList<Tovar> curTb, QDate dt1, QDate dt2, int methodSell);
    QList<Tovar> apply_sell_filter(QList<Tovar> curTb);

    QVector<Tovar> get_sold_tovars();
    QVector<Tovar> get_sold_tovars(QDate dt1, QDate dt2);

    QDate get_date_start() {
        return dateStart->selectedDate();
    }
    QDate get_date_end() {
        return dateStop->selectedDate();
    }
    void setSellFileName(QString name) {
        sellFileName = name;
    }
    QString getSellFileName() {
        return sellFileName;
    }

    void setOpisateli(QStringList lst) {
        sellOpisateli = lst;
        sellNomerBox->addItems(sellOpisateli);
        sellDateBox->addItems(sellOpisateli);
        sellTimeBox->addItems(sellOpisateli);
        sellKolBox->addItems(sellOpisateli);
    }
    QStringList getOpisateli() {
        return sellOpisateli;
    }

    void setNomberBoxIndex(int idx) {
        sellNomerBox->setCurrentIndex(idx);
    }
    int getNomberBoxIndex() {
        return sellNomerBox->currentIndex();
    }

    void setDateBoxIndex(int idx) {
        sellDateBox->setCurrentIndex(idx);
    }
    int getDateBoxIndex() {
        return sellDateBox->currentIndex();
    }

    void setTimeBoxIndex(int idx) {
        sellTimeBox->setCurrentIndex(idx);
    }
    int getTimeBoxIndex() {
        return sellTimeBox->currentIndex();
    }

    void setKolBoxIndex(int idx) {
        sellKolBox->setCurrentIndex(idx);
    }
    int getKolBoxIndex() {
        return sellKolBox->currentIndex();
    }

    void setLastKnownFile(QString fl) {
        last_known_file = fl;
    }
    QString getLastKnownFile() {
        return last_known_file;
    }

    void setLastKnownPos(int pos) {
        last_known_pos = pos;
    }
    int getLastKnownPos() {
        return last_known_pos;
    }

    void setLastKnownDate(QDate dt) {
        last_known_date = dt;
    }
    QDate getLastKnownDate() {
        return last_known_date;
    }
    void setSellMethod(int index) {
        methodSellBox->setCurrentIndex(index);
    }

    void on_clear_last_button_clicked(QSettings *m_settings);
    void updateSellTab();

    //по-хорошему конечно так не делается, но мне уже лень писать, я скоро увольняюсь
    QComboBox *sellNomerBox, *sellDateBox, *sellTimeBox, *sellKolBox;
    QCalendarWidget *dateStart, *dateStop;


private slots:
    void on_selectSellFileButtonClicked();
    void on_clear_last_button_clicked();
    int foundStartPos(QDate dt1, int init_pos);
    int foundEndPos(QDate dt2, int init_pos);
    int foundInitPos(QDate dt1);

private:
    //QWidget *tab7;
    QGridLayout *layTab7;
    QLabel *lb1, *lb2, *lb3, *lb4, *lb5, *lb6, *lb7, *lb8, *lb9, *lb10;
    QPushButton *selectSellFileButton;
    QPushButton *saveSellSettingsButton;
    QString sellFileName;
    QStringList sellOpisateli;
    QComboBox *methodSellBox;
    //QLineEdit *methodSellValue;
    bool sell_file_is_checked;
    Tinformer *sell_informer;
    QDate last_known_date;
    int last_known_pos;
    QString last_known_file;
    QPushButton *clear_last_button;
    QProgressBar *progressBar;
    QStatusBar *statusBar;

    //methodSellBox->setCurrentIndex(1);
    //int methodSell = methodSellBox->currentIndex();
    //0 - заменить количество товара количеством проданных единиц
    //1 - вычесть из старого количества количество проданных единиц
    //2 - определение среднего за день (заносим в поле Цена2) и количества проданного


};


#endif // SELLFILTERWIDGET_H
