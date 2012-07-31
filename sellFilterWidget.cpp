#include "sellFilterWidget.h"


SellFilterWidget::SellFilterWidget(QToolBar *ui_mainToolBar,
                                   QStatusBar *statBar,
                                   QWidget *pwgt) {
    sell_file_is_checked = false;
    statusBar = statBar;

    layTab7 = new QGridLayout;

    selectSellFileButton = new QPushButton (tr("Select file with sell records"));
    layTab7->addWidget(selectSellFileButton, 0, 0);
    connect(selectSellFileButton, SIGNAL(clicked()), SLOT(on_selectSellFileButtonClicked()));

    lb1 = new QLabel(tr("Selected file: "));
    lb2 = new QLabel("");

    layTab7->addWidget(lb1, 0, 1, 1, 3);
    //layTab7->addWidget(lb2, 0, 2, 1, 3);

    clear_last_button = new QPushButton(tr("Clear last pos"));
    connect (clear_last_button, SIGNAL(clicked()),
             SLOT(on_clear_last_button_clicked()));
    layTab7->addWidget(clear_last_button, 1, 3);

    sellNomerBox = new QComboBox;
    sellDateBox = new QComboBox;
    sellTimeBox = new QComboBox;
    sellKolBox = new QComboBox;

    lb3 = new QLabel(tr("Tnomer"));
    lb4 = new QLabel(tr("Date of sell"));
    lb5 = new QLabel(tr("Time of sell"));
    lb6 = new QLabel(tr("Quantity"));

    layTab7->addWidget(lb3, 1, 0);
    layTab7->addWidget(sellNomerBox, 1, 1);
    layTab7->addWidget(lb4, 2, 0);
    layTab7->addWidget(sellDateBox, 2, 1);
    layTab7->addWidget(lb5, 3, 0);
    layTab7->addWidget(sellTimeBox, 3, 1);
    layTab7->addWidget(lb6, 4, 0);
    layTab7->addWidget(sellKolBox, 4, 1);

    lb10 = new QLabel(tr("Sells: " ));
    methodSellBox = new QComboBox;
    methodSellBox->insertItems(0, QStringList()
                               << tr("Replace")
                               << tr("Minus")
                               << tr("Per day"));
    //methodSellValue = new QLineEdit("0");

    //layTab7->addWidget(lb10, 5, 0);
    //layTab7->addWidget(methodSellBox, 5, 1);
    ui_mainToolBar->addWidget(lb10);
    ui_mainToolBar->addWidget(methodSellBox);
    ui_mainToolBar->addSeparator();

    //layTab7->addWidget(methodSellValue, 5, 2);

    dateStart = new QCalendarWidget;
    dateStop = new QCalendarWidget;

    dateStart->setFirstDayOfWeek ( Qt::Monday );
    dateStop->setFirstDayOfWeek ( Qt::Monday );

    dateStart->setGridVisible(true);
    dateStop->setGridVisible(false);

    dateStart->setSelectedDate(QDate::currentDate());
    dateStop->setSelectedDate(QDate::currentDate());

    lb7 = new QLabel(tr("From date: "));
    lb8 = new QLabel(tr("To date: "));

    layTab7->addWidget(lb7, 6, 1);
    layTab7->addWidget(dateStart, 7, 1);
    layTab7->addWidget(lb8, 6, 3);
    layTab7->addWidget(dateStop, 7, 3);


    //tab7->setLayout(layTab7);
    this->setLayout(layTab7);
}
SellFilterWidget::~SellFilterWidget() {

}

void SellFilterWidget::check_sell_file() {
    qDebug() << "Checking sell file" << sellFileName;
    sell_informer = new Tinformer;
    DbfConfig sellDb;
    sellDb.fileName = sellFileName;

    QFileInfo fi;
    fi.setFile(sellFileName);
    if (!fi.exists()) {
        qDebug() << "File not exists";
        return;
    }


    sell_informer->prepare(&sellDb);

    sell_informer->set_tb_name(sellDb.fileName);

    int j1 = sellNomerBox->currentIndex();
    int j2 = sellDateBox->currentIndex();
    int j3 = sellTimeBox->currentIndex();
    int j4 = sellKolBox->currentIndex();

    sellNomerBox->clear();
    sellDateBox->clear();
    sellTimeBox->clear();
    sellKolBox->clear();

    sellOpisateli = sell_informer->tb_describe("DBF");

    sellNomerBox->addItems(sellOpisateli);
    sellDateBox->addItems(sellOpisateli);
    sellTimeBox->addItems(sellOpisateli);
    sellKolBox->addItems(sellOpisateli);

    sellNomerBox->setCurrentIndex(j1);
    sellDateBox->setCurrentIndex(j2);
    sellTimeBox->setCurrentIndex(j3);
    sellKolBox->setCurrentIndex(j4);

    lb1->setText(lb1->text() + "; max=" + QString::number(sell_informer->get_maximum()));

    sell_file_is_checked = true;
}

QList<Tovar> SellFilterWidget::apply_sell_filter(QList<Tovar> curTb) {
    return this->apply_sell_filter(curTb, dateStart->selectedDate(),
                            dateStop->selectedDate(), methodSellBox->currentIndex());
}

int SellFilterWidget::foundInitPos(QDate dt1) {
    int init_pos;
    if (dt1 >= last_known_date) {
        qDebug() << "Using last known position";
        init_pos = last_known_pos;
    } else {
        qDebug() << "Found last date";
        init_pos = 0;
    }
    return init_pos;
}

int SellFilterWidget::foundStartPos(QDate dt1, int init_pos) {
    QString curDate;
    QList<Tovar> tb1;
    int startPos;
    curDate =  dt1.toString("yyyyMMdd");
    tb1 = sell_informer->info(curDate, "tbarcode", init_pos, -1, 1, true);

    if (!tb1.count()) {
        startPos = 0;
    } else {
        startPos = sell_informer->last_found_record_number();
        startPos--;
        qDebug() << "Start pos is " << startPos;
        QString dt = tb1.at(0).barcode;
        QDate ddt = QDate::fromString(dt, "yyyyMMdd");
        QDate dd2 = last_known_date.addDays(14);
        qDebug() << "Before start is " << ddt << "\n" << dd2;
        if (ddt > dd2) {
            ddt = ddt.addDays(-14);
            tb1 = sell_informer->info(ddt.toString("yyyyMMdd"), "tbarcode", init_pos, -1, 1, true);

            last_known_pos = sell_informer->last_found_record_number();
            last_known_date = ddt;
            last_known_file = sellFileName;
            qDebug() << "We know now: " << last_known_pos << last_known_date;
        }
    }
    return startPos;
}

int SellFilterWidget::foundEndPos(QDate dt2, int init_pos) {
    QString curDate = dt2.toString("yyyyMMdd");
    QList<Tovar> tb2;
    tb2 = sell_informer->info(curDate, "tbarcode", init_pos, -1, 0, false);
    int lastPos;
    int max = sell_informer->get_maximum();
    if (tb2.count()) {
        lastPos = sell_informer->last_found_record_number();
        lastPos++;
        lastPos++;
        qDebug() << "Last pos " << lastPos;
        if (lastPos > max) lastPos = max;

    } else {
        lastPos = max;
    }
    return lastPos;
}

QList<Tovar> SellFilterWidget::apply_sell_filter(QList<Tovar> curTb, QDate dt1, QDate dt2, int methodSell) {
    //int methodSell
    //0 - заменить количество товара количеством проданных единиц
    //1 - вычесть из старого количества количество проданных единиц
    //2 - определение среднего за день (заносим в поле Цена2) и количества проданного

    if (!sell_file_is_checked) check_sell_file();

    dbTranslator sellConfig;
    sellConfig.tbarcode = sellDateBox->currentText();
    sellConfig.tnomer =   sellNomerBox->currentText();
    sellConfig.tname =    sellTimeBox->currentText();
    sellConfig.tprice =   sellKolBox->currentText();

    sell_informer->set_fields(&sellConfig);

    sell_informer->set_limit_search(1);


    QString curDate;
    QList<Tovar> tb1, tb2, tb3, newTb;
    int init_pos = foundInitPos(dt1);
    //qDebug() << "Last known date is " << last_known_date;

    int startPos = foundStartPos(dt1, init_pos);
    int lastPos = foundEndPos(dt2, init_pos);
    qDebug() << "start-end: " << startPos << lastPos;

    int period = dt1.daysTo(dt2) +1;
    int count, prod;
    Tovar tovar;
    QList<QDateTime> lastSellDates;
    QDateTime lastSellDate;
    QString lh, lm ;
    QDateTime zeroDate = QDateTime(QDate::fromString("19000101", "yyyyMMdd"));

    progressBar = new QProgressBar();
    progressBar->setMaximum(curTb.count());
    statusBar->addWidget(progressBar);
    progressBar->show();

    for (int i = 0; i<curTb.count(); i++) {
        progressBar->setValue(i);
        tovar = curTb.at(i);
        tb3 = sell_informer->info
                (QString::number(tovar.nomer_of_tovar),
                 "tnomer",
                 startPos,
                 lastPos,
                 -1,
                 true,
                 false);

        QApplication::processEvents();
        count = 0;
        prod = tb3.count();
        for (int j = 0; j<prod; j++) {
            count += tb3.at(j).price1;
        }
        if (prod) {
            lh = tb3.at(prod-1).name_of_tovar.split(":").at(0);
            lm = tb3.at(prod-1).name_of_tovar.split(":").at(1);
            //qDebug() << "Time is " <<lh << lm;

            lastSellDate = QDateTime(QDate::fromString(tb3.at(prod-1).barcode, "yyyyMMdd"),
                                     QTime::fromString(tb3.at(prod-1).name_of_tovar, "hh:mm"));
            //qDebug() << "Last date " << lastSellDate;
        } else {
            lastSellDate = zeroDate;
        }
        lastSellDates << lastSellDate;
        //в зависимости от метода обновления - разные действия
        int days;
        switch (methodSell) {
        case 0:
            tovar.quantity = count;
            break;
        case 1:
            tovar.quantity = tovar.quantity - count;
            break;
        case 2:
            if (tovar.quantity) {
                tovar.price2 = (float)count / period;
            } else {
                days = dt1.daysTo(lastSellDate.date()) + 1;
                tovar.price2 = (float)count / days;
            }
            tovar.quantity = count;
            break;
        default:
            tovar.quantity = count;
            break;
        }

        newTb << tovar;
    }
    if (curTb.count() == 1 && tb3.count()) {
        //был поиск продаж только одного товара и он продавался -
        //  покажем информацию о дате и времени продажи
        int prod = tb3.count();
        QString dt = tb3.at(prod-1).barcode;
        QDate ldt  = QDate::fromString(dt, "yyyyMMdd");
        dt = ldt.toString("d MMMM yyyy, dddd");
        QString tm = tb3.at(prod-1).name_of_tovar;
        QString message = tr("Total sold %1 goods (%2 pices) of %3 tnomer%4Last date is %5 at %6")
                .arg(QString::number(prod))
                .arg(QString::number(newTb.at(0).quantity))
                .arg(QString::number(curTb.at(0).nomer_of_tovar))
                .arg("\n")
                .arg(dt)
                .arg(tm);
        ListFoundedItemsDialog *dlg = new ListFoundedItemsDialog(this);
        dlg->setMessage(message);
        dlg->exec();
    }
    statusBar->removeWidget(progressBar);
    delete progressBar;

    return newTb;
}

void SellFilterWidget::on_selectSellFileButtonClicked() {
    //выбор DBF-файла
    QString str = QFileDialog::getOpenFileName(0, "Select SELL file", sellFileName, "DBF-files (*.dbf)");
    sellFileName = str;
    updateSellTab();
    check_sell_file();
}

void SellFilterWidget::updateSellTab() {
    lb1->setText(lb1->text() + sellFileName +
                 last_known_date.toString(" dd MM yyyy, ") +
                 QString::number(last_known_pos));
    qDebug() << "last date " << last_known_date;

}

void SellFilterWidget::on_clear_last_button_clicked() {
    last_known_pos = 0;
    last_known_date = QDate::currentDate().addYears(-50);
    last_known_file = "";
}

QVector<Tovar> SellFilterWidget::get_sold_tovars() {
    ///Возвращает список проданных товаров в период дат, указанных пользователем в виджетах-календариках
    return this->get_sold_tovars(dateStart->selectedDate(),
                                 dateStop->selectedDate());
}

QVector<Tovar> SellFilterWidget::get_sold_tovars(QDate dt1, QDate dt2) {
    ///Возвращает список проданных товаров между датами dt1 и dt2
    if (!sell_file_is_checked) check_sell_file();
    dbTranslator sellConfig;
    sellConfig.tbarcode = sellDateBox->currentText();
    sellConfig.tnomer =   sellNomerBox->currentText();
    sellConfig.tname =    sellTimeBox->currentText();
    sellConfig.tprice =   sellKolBox->currentText();

    sell_informer->set_fields(&sellConfig);

    sell_informer->set_limit_search(0);


    int init_pos = this->foundInitPos(dt1);
    int startPos = this->foundStartPos(dt1, init_pos);
    int endPos = this->foundEndPos(dt2, init_pos);
    qDebug() << "start-end: " << startPos << endPos;

    QList<Tovar> tb;
    QVector<Tovar> vtb;
    tb = sell_informer->info("", "tname", startPos, endPos);
    vtb = tb.toVector();
    qDebug() <<"Found items: " << tb.toVector().count();
    for (int i = 0; i<tb.count(); i++) vtb[i].quantity = vtb[i].price1;

    return vtb;
}
