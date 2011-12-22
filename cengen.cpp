#include "cengen.h"
#include "ui_cengen.h"
#include "tinformer.h"
#include "barcode.h"
#include <QDebug>
#include <QFileDialog>
//#include <QGraphicsLineItem>
#include "cen_viewer.h"


cengen::cengen(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::cengen)
{
    ui->setupUi(this);

    ui_mainToolBar = qFindChild<QToolBar*>(this, "mainToolBar");
    ui_action3 = qFindChild<QAction*>(this, "action_3");
    ui_action3->setIcon(QIcon(":/share/images/tmp.ico"));

    ui_mainToolBar->addAction(ui_action3);


    ui_lineEdit = qFindChild<QLineEdit*>(this,"lineEdit");
    ui_lineW = qFindChild<QLineEdit*>(this,"lineW");
    ui_lineH = qFindChild<QLineEdit*>(this,"lineH");
    ui_lineW_2 = qFindChild<QLineEdit*>(this,"lineW_2");
    ui_lineH_2 = qFindChild<QLineEdit*>(this,"lineH_2");

    ui_lineHost = qFindChild<QLineEdit*>(this,"lineHost");
    ui_lineName = qFindChild<QLineEdit*>(this,"lineName");
    ui_linePort = qFindChild<QLineEdit*>(this,"linePort");
    ui_lineUser = qFindChild<QLineEdit*>(this,"lineUser");
    ui_linePass = qFindChild<QLineEdit*>(this,"linePass");

    ui_groupDBF = qFindChild<QGroupBox*>(this, "groupDBF");
    ui_groupSQL = qFindChild<QGroupBox*>(this, "groupSQL");

    ui_spinLimit = qFindChild<QSpinBox*>(this, "spinLimit");

    ui_radioButton = qFindChild<QRadioButton*>(this,"radioButton");
    ui_radioButton_3 = qFindChild<QRadioButton*>(this,"radioButton_3");
    ui_radioButton_4 = qFindChild<QRadioButton*>(this,"radioButton_4");
    ui_radioButton_5 = qFindChild<QRadioButton*>(this, "radioButton_5");


    ui_label = qFindChild<QLabel*>(this, "label");
    ui_statusLabel = qFindChild<QLabel*>(this, "statusLabel");
    ui_countLabel = qFindChild<QLabel*>(this, "countLabel");
    ui_labelDBFname = qFindChild<QLabel*>(this, "labelDBFname");

    ui_tabWidget = qFindChild<QTabWidget*>(this, "tabWidget");
    ui_tableWidget = qFindChild<QTableWidget*>(this, "tableWidget");
    ui_scrollArea = qFindChild<QScrollArea*>(this,"scrollArea");
    ui_comboBox = qFindChild<QComboBox*>(this,"comboBox");
    ui_comboTbList = qFindChild<QComboBox*>(this,"comboTbList");

    ui_comboTnomer = qFindChild<QComboBox*>(this, "comboTnomer");
    ui_comboTname = qFindChild<QComboBox*>(this, "comboTname");
    ui_comboTbarcode = qFindChild<QComboBox*>(this, "comboTbarcode");
    ui_comboTprice = qFindChild<QComboBox*>(this, "comboTprice");

    ui_zoomInButton = qFindChild<QPushButton*>(this,"zoomInButton");
    ui_zoomOutButton = qFindChild<QPushButton*>(this,"zoomOutButton");
    ui_save_db_config_button = qFindChild<QPushButton*>(this,"save_db_config_button");

    previewLayout = new QVBoxLayout(ui_scrollArea);


    this->ui_comboTbList->setEnabled(false);

    this->new_line_ready();

    editing_price2 = false;

    my_informer = new Tinformer();
    zoom = 1.0;

    method = "tbarcode";
    //db_source = 1;


    this->paperOrientation = "portrate";

    //устанавливаем валидатор (первый вход в программу - поиск через баркод)
    ui_bvalidator = new BarcodeValidator(this);
    ui_nvalidator = new QIntValidator(this);
    ui_svalidator = new SqlValidator(this);
    ui_lineEdit->setValidator(ui_bvalidator);

    //конфигурим фильтр
    ui_filterBox = qFindChild<QGroupBox*>(this, "filterBox");
    filter_is_on = false;
    filterInformer = new Tinformer();
    ui_filterWhatBox = qFindChild<QComboBox*>(this, "filterWhatBox");
    ui_filterWhereBox = qFindChild<QComboBox*>(this, "filterWhereBox");
    ui_filterWhatToFoundBox = qFindChild<QComboBox*>(this, "filterWhatToFoundBox");
    ui_filterMethodBox = qFindChild<QComboBox*>(this, "filterMethodBox");
    ui_filterLineText = qFindChild<QLineEdit*>(this, "filterLineText");


    this->readSettings();

    this->set_tableWidget_header(ui_tableWidget);

    //подготовка сцены для рендеринга ценников
    currentScene = new QGraphicsScene;
    view = new QGraphicsView();
    view->setScene(currentScene);
    previewLayout->addWidget(view);

    //подготовка редактора шаблонов
    shablon_editor = new editor(this);
    connect(shablon_editor, SIGNAL(shablon_is_ready(QDomDocument)), SLOT(describe_shablon(QDomDocument)));



}

cengen::~cengen()
{
    delete ui;
}

void cengen::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void cengen::on_exitButton_clicked()
{
    this->writeSettings();
    close();
}

void cengen::tovar_search() {
    Tovar tovar;
    QList<Tovar> tovarListFull;
    tovarListFull = my_informer->info(ui_lineEdit->text(), this->method);

    if (filter_is_on) {
        tovarListFull = apply_filter(tovarListFull);
    }

    QList<Tovar> tovarList;

    if (tovarListFull.count() != 1) {
        tovarList = this->show_found_items(tovarListFull, "check");
    } else {
        tovarList = tovarListFull;
    }

    load_tovar_list_into_cengen(tovarList);
}

void cengen::load_tovar_list_into_cengen(QList<Tovar> tovarList) {
    //загружаем список товаров в таблицу генератора ценников
    add_flag = true;

    for (int i = 0; i<tovarList.count(); i++) {
        //qDebug() << "i=" << i;
        Tovar tovar = tovarList.at(i);

        int position = ui_tableWidget->rowCount();
        //qDebug() << "position=" << position;
        ui_tableWidget->setRowCount(position + 1);
        ui_countLabel->setText(QString::number(ui_tableWidget->rowCount()));

        add_table_item(ui_tableWidget, position, tovar);

            QTableWidgetItem* itemDel = new QTableWidgetItem("x");
            ui_tableWidget->setItem(position, 6, itemDel);
            ui_tableWidget->item(position, 6)->setToolTip("DELETE");
            ui_tableWidget->item(position, 6)->setWhatsThis("Delete line from table");

            //ui_tableWidget->scrollToItem(itemDel);
            ui_tableWidget->scrollToBottom();
    }
    add_flag = false;

}

QList<Tovar> cengen::show_found_items(QList<Tovar> inputList, QString method) {
    QList<Tovar> tovarList;
    QTableWidget* table = new QTableWidget;
    MyDialog* dlg = new MyDialog;

    if (!inputList.count()) {
        //если пусто - показать ошибку
        dlg->setTable("NOT FOUND");

    } else {
        table->setRowCount(inputList.count());
        table->setColumnCount(7);
        set_tableWidget_header(table);
        Tovar tovar;
        for (int i = 0; i<inputList.count(); i++) {
            tovar = inputList.at(i);
            //int position = table->rowCount();
            //table->setRowCount(position + 1);

            add_table_item(table, i, tovar);


            QTableWidgetItem* itemCheck = new QTableWidgetItem(" ");
            table->setItem(i, 6, itemCheck);
            table->item(i, 6)->setToolTip("SELECT");
            table->item(i, 6)->setWhatsThis("Select item to list");
        }
        dlg->setTable(&table);
    }
    if (dlg->exec()) {
        tovarList = get_tovar_list(table, "V");
    }

    return tovarList;
}

void cengen::on_pushButton_clicked()
{
    this->tovar_search();
    this->new_line_ready();
}

void cengen::on_lineEdit_returnPressed()
{
    if (ui_lineEdit->text() != "") {
        this->tovar_search();
        this->new_line_ready();
    } else {
        //нажата Энтер с пустым полем - значит, хотим отредактировать
        //    цену только что введенного товара
        editing_price2 = true;
        if (ui_tableWidget->rowCount()) {
            QTableWidgetItem* item = ui_tableWidget->item
                    (ui_tableWidget->rowCount()-1, 5);
            ui_tableWidget->setCurrentItem(item);
            ui_tableWidget->setFocus();
        }
    }

}

void cengen::new_line_ready() {
    ui_lineEdit->clear();
    ui_lineEdit->setFocus();
}

void cengen::on_pushButton_2_clicked()
{
    //функция для формирования ценников

    //первым делом - получить из таблицы все поля в QList
    //qDebug() << "SFORM";
    //ui_tabWidget->setMovable(true);
    ui_tabWidget->setCurrentIndex(2);
    this->generate_preview();

}

QList<Tovar> cengen::get_tovar_list(QTableWidget *table, QString priznak) {
    QList<Tovar> spisok;
    Tovar tovar;
    //qDebug() << "total " << ui_tableWidget->rowCount();
    for (int i = 0; i<table->rowCount(); i++) {
        //qDebug() << "row " << i;
        if (table->item(i, 6)->text() == priznak) {
            tovar.name_of_tovar = table->item(i, 1)->text().trimmed();
            tovar.barcode = table->item(i, 2)->text().trimmed();
            tovar.nomer_of_tovar = table->item(i, 3)->text().toInt();
            tovar.price1 = table->item(i, 4)->text().toFloat();
            if (!(table->item(i, 5)->text().isEmpty())) {
                tovar.price2 = table->item(i, 5)->text().toFloat();
            }
            spisok << tovar;
        }
    }
    //qDebug() << "spisok done" << spisok.count();
    return spisok;
}

void cengen::on_pushButton_3_clicked()
{
    //функция выбора шаблона файла


    QString open_name;
    if (file_is_ready) {
        open_name = file.fileName();
    } else {
        open_name = "";
    }

    QString str = QFileDialog::getOpenFileName(0, tr("Select shablon file"), open_name, tr("CEN-files (*.cen)"));

    if (file_is_ready && str == "") return;

    if (str == "") {
        ui_label->setText("Please select file");
        //this->file_is_ready = false;
    } else {
        file.setFileName(str);
        this->read_file_shablon();
        file_is_ready = true;
    }
}

void cengen::read_file_shablon() {
        QDomElement domElement;
        if (file.open(QIODevice::ReadOnly)) {
        if (domDoc.setContent(&file)) {
            domElement = domDoc.documentElement();
            file_is_ready = true;
        }
    } else {
        file_is_ready = false;
    }
    this->describe_shablon(domDoc);

}

void cengen::describe_shablon(QDomDocument shablon) {

    domDoc = shablon;
    this->shablonElement = domDoc.documentElement();
    //qDebug() << "SHABLON\n" << domDoc.toString();
    rectCen = this->get_shablon_rect(shablonElement);
    ui_label->setText(get_shablon_name(shablonElement));
    this->update_values();
    //this->shablonElement = shablon;
}

void cengen::on_tabWidget_currentChanged(int index)
{
    if (!index) {
        this->new_line_ready();
        return;
    }
    update_values();

}

void cengen::on_pushButton_4_clicked()
{
    this->generate_preview();

}

QRectF cengen::get_shablon_rect(const QDomNode &node) {
    QRectF rect;// = new QRectF();
    rect.setWidth(10);
    rect.setHeight(10);
    
    if (!file_is_ready) {
            return rect;
    }
    QDomNode domNode = node.firstChild();
    while(!domNode.isNull()) {
        if(domNode.isElement()) {
           QDomElement domElement = domNode.toElement();
           if(!domElement.isNull()) {
               if (domElement.tagName() == "base") {
                   //описание размера шаблона
                   qDebug() << "We read rect of shablon";
                   QString width = domElement.attribute("width", "");
                   rect.setWidth(width.toFloat());
                   QString heith = domElement.attribute("heith", "");
                   rect.setHeight(heith.toFloat());
                   return rect;
               }

           }
        }
        //get_shablon_rect(domNode);
        domNode = domNode.nextSibling();
     }


    return rect;
}

QString cengen::get_shablon_name(const QDomNode &node) {
    QString name = tr("Select proper file");

    if (!file_is_ready) {
        qDebug() << "Not ready";
            return name;
    }
    QDomNode domNode = node.firstChild();
    while(!domNode.isNull()) {
        //qDebug() << "Not null";
        if(domNode.isElement()) {
            //qDebug() << "is element";
           QDomElement domElement = domNode.toElement();
           if(!domElement.isNull()) {
               if (domElement.tagName() == "name") {
                   //название шаблона
                   name = domElement.text();
                   return name;
               }
               qDebug() << "element " << domElement.tagName();

           }
        }
        //get_shablon_name(domNode);
        domNode = domNode.nextSibling();
     }

    return name;

}

void cengen::on_comboBox_activated(QString paper)
{
    qDebug() << "paper" << paper;
    update_values();
}

void cengen::update_values() {
    switch (ui_comboBox->currentIndex()) {
    case 0:
        //qDebug() << "paperA4";
        this->pageW = 2100;
        this->pageH = 2970;
        this->pageSize = "A4";
        break;
    case 1:
        //qDebug() << "paperA5";
        this->pageW = 1480;
        this->pageH = 2100;
        this->pageSize = "A5";
        break;
    default:
        this->pageW = 2100;
        this->pageH = 2970;
        this->pageSize = "A4";
        break;
    }

    qDebug() << "orient" << paperOrientation;
    if (paperOrientation == "landscape") {
        float tmp = this->pageW;
        this->pageW = this->pageH;
        this->pageH = tmp;
    }

    if (file_is_ready) {
        this->Crows = pageH / rectCen.height();
        this->Ccols = pageW / rectCen.width();
    } else {
        Crows = 1;
        Ccols = 1;
    }

    ui_lineW->setText(QString::number(pageW));
    ui_lineH->setText(QString::number(pageH));
    ui_lineW_2->setText(QString::number(Ccols));
    ui_lineH_2->setText(QString::number(Crows));
    //qDebug() << "/dated";


}

void cengen::generate_preview() {
    if (!file_is_ready) {
        qDebug() << "File is not ready";
        ui_statusLabel->setText("File is not ready");
        return;
    }

    this->switch_zoom_buttons_enabled(true);

    currentScene->clear();

    pages.clear();

    QList<Tovar> spisok =  this->get_tovar_list(ui_tableWidget, "x");
    Tovar currentTovar;

    //высчитываем левый верхний угол первой страницы
    float bXpos, bYpos, bXstart, bYstart;
    bXstart = (pageW - Ccols*rectCen.width())/2;
    bYstart = (pageH - Crows*rectCen.height())/2;
    bXpos = bXstart;
    bYpos = bYstart;

    //заносим этот угол в список страниц
    QRectF page(0, 0, pageW, pageH);
    pages << page;
    //currentScene->addRect(pages.at(0));

    for (int i = 0; i<spisok.count(); i++) {

        //получаем из списка очередной товар и рисуем на него ценник
        currentTovar = spisok.at(i);
        currentTovar.name_of_tovar = currentTovar.name_of_tovar.toUtf8();
        Cennic* cennic = new Cennic(&currentTovar, shablonElement);
        QPoint corner = cennic->render(currentScene, bXpos, bYpos);

        if ( ((i+1) % this->Ccols) != 0) {
            bXpos += corner.x();
        } else {
            //qDebug() << "Next line";
            bXpos = bXstart;
            bYpos += corner.y();
        }

        if ( !((i+1) % (Crows * Ccols)) ) {
            //начинаем новую страницу
            QRectF page(pageW*pages.count()-1, 0,
                        pageW, pageH);
            pages << page;
            bXstart += pageW;
            bXpos = bXstart;
            //bYstart не меняется - делаем все странички в строчку
            bYpos = bYstart;
        }


    }

    view->scale(0.9, 0.9);

    qDebug() <<  "There is pages: " << pages.count();

}

void cengen::on_zoomInButton_clicked()
{
    view->scale(2, 2);
    zoom = zoom*2.0;
}

void cengen::on_zoomOutButton_clicked()
{
    view->scale(0.5, 0.5);
    zoom = zoom/2.0;
}

void cengen::switch_zoom_buttons_enabled(bool state = false) {
    ui_zoomInButton->setEnabled(state);
    ui_zoomOutButton->setEnabled(state);
}

void cengen::on_printButton_clicked()
{
    //qDebug() << "Point X and Y is: " << pages.at(0).width() << "; " << pages.at(0).height();


    QPrinter printer(QPrinter::HighResolution);

    if (pageSize == "A4") {
        printer.setPageSize(QPrinter::A4);
    }
    if (pageSize == "A5") {
        printer.setPageSize(QPrinter::A5);
    }

    if (paperOrientation == "landscape") {
        printer.setOrientation(QPrinter::Landscape);
    } else {
        printer.setOrientation(QPrinter::Portrait);
    }


    printer.setFullPage(true);

    printer.setNumCopies(1);

    printer.setColorMode(QPrinter::GrayScale);


    QPrintDialog dlg (&printer, this);
    dlg.setMinMax(1, pages.count());
    dlg.setFromTo(1, pages.count());

    if (dlg.exec() == QDialog::Accepted)
    {
        printer.setColorMode(QPrinter::GrayScale);
        QPainter painter(&printer);
        QRectF region = printer.pageRect();

        int min, max;
        min = dlg.fromPage();
        max = dlg.maxPage();

        if (min > pages.count()) {
            min--;
        }


        qDebug() << "min max pages " << min << max;

        for (int i = min; i< max; i++) {
            QRect current = pages.at(i).toRect();
            currentScene->render(&painter, region, current, Qt::KeepAspectRatio);
            if (i<(max-1)) {
                printer.newPage();
            }

        }

    }
}

void cengen::writeSettings()
{
        m_settings.beginGroup("/Settings");
        if (this->db_source) {
            m_settings.setValue("/source", 1);
            //если 1 - ищем в мускуле, если 0 - в DBF
            if (db_is_ready) {
                m_settings.beginGroup("/sql");
                m_settings.setValue("/host", sql->dbHost);
                m_settings.setValue("/dbName", sql->dbName);
                m_settings.setValue("/pass", sql->dbPass);
                m_settings.setValue("/user", sql->dbUser);
                m_settings.setValue("/tbName", sql->tbName);

                //qDebug() << "saving TABLE " << sql->tbName;
                m_settings.endGroup();
                qDebug() << "sql config written";
            }
        } else {
            m_settings.setValue("/source", 0);
            m_settings.beginGroup("/dbf");
            m_settings.setValue("/filename", this->dbf->fileName);
            m_settings.endGroup();
        }

        if (file_is_ready) {

        m_settings.setValue("/shablon", file.fileName());
        //qDebug() << "Settings written";
    }

    if (describer_is_ready) {
        m_settings.beginGroup("/describer");
        m_settings.setValue("/tnomer", config_index[0]);
        m_settings.setValue("/tname", config_index[1]);
        m_settings.setValue("/tbarcode", config_index[2]);
        m_settings.setValue("/tprice", config_index[3]);
        //qDebug() << "writing index of 3 = " << config_index[3];
        m_settings.endGroup();
        //qDebug() << "desc written";
    }

    //записываем настройки фильтра
    m_settings.setValue("/filterIsOn", filter_is_on);
    if (filter_is_on) {

        m_settings.beginGroup("/filter");

        //записываем название файла
        m_settings.setValue("/FileName", filterDbf.fileName);

        //следующие два поля - это название полей из предыдушего файла
        m_settings.setValue("/WhatBox", ui_filterWhatBox->currentIndex());
        m_settings.setValue("/WhereBox", ui_filterWhereBox->currentIndex());

        //строка поиска
        m_settings.setValue("/LineText", ui_filterLineText->text());

        //ключ поиска - товарный номер, штрихкод и т.п. (в виде индекса)
        m_settings.setValue("/WhatToFoundBox", ui_filterWhatToFoundBox->currentIndex());

        //метод фильтрации - равно, больше, меньше и т.п. (в виде индекса)
        m_settings.setValue("/Method", ui_filterMethodBox->currentIndex());

        m_settings.endGroup();

    }

    m_settings.setValue("tabIndex", ui_tabWidget->currentIndex());

    m_settings.endGroup();

}

void cengen::readSettings() {

    static int i = 0;
    i++;

    qDebug() << "Group is " << m_settings.group();

    //добавить, если там нет названия шаблона
    QString fileName = m_settings.value("/Settings/shablon", "").toString();
    file.setFileName(fileName);
    this->read_file_shablon();

    //создаем новые переменные для работы
    this->dbf = new DbfConfig;
    this->sql = new SqlConfig;

    //читаем источник данных (по умолчанию - DBF)
    this->db_source = m_settings.value("/Settings/source", "0").toInt();

    if (db_source) {
        //если 1 - ищем в мускуле
        ui_radioButton->setChecked(true);
        //ui_radioButton_4->setChecked(true);

        //блокируем блок выбора файла DBF и включаем блок настроек MySQL
        ui_groupSQL->setEnabled(true);
        ui_groupDBF->setEnabled(false);

        //читаем настройки базы данных
        //m_settings.beginGroup("/Settings/sql");
        sql = new SqlConfig;
        sql->dbHost = m_settings.value("/Settings/sql/host", "").toString();
        sql->dbName = m_settings.value("/Settings/sql/dbName", "").toString();
        sql->dbUser = m_settings.value("/Settings/sql/user", "").toString();
        sql->dbPass = m_settings.value("/Settings/sql/pass", "").toString();
        sql->tbName = m_settings.value("/Settings/sql/tbName", "").toString();
        //m_settings.endGroup();

        //передаем эти настройки в информер
        this->db_is_ready = my_informer->prepare(sql);

        if (db_is_ready) {
            //если база данных открывается
            //читаем список таблиц из базы данных
            update_ui_connection_established();

            if (sql->tbName != "") {
                //если в настройках - не пустое имя таблицы
                my_informer->set_tb_name(sql->tbName);
                this->opisateli.clear();
                opisateli = my_informer->tb_describe(sql->tbName);

                this->set_opisateli_from_settings();
                this->update_ui_db_controls();

            } else {
                //устанавливаем фокус на выборе таблицы
            }

        } else {
            //если база данных не открывается -
            //  блокируем остальные контролы,
            //  устанавливаем фокус ввода на настройках базы данных

            return;
        }


    } else {
        //значит, db_source = 0
        //и мы используем DBF-файлы
        qDebug() << "we read DBF settings";
        my_informer->prepare(dbf);
        ui_radioButton->setChecked(false);
        ui_radioButton_4->setChecked(true);

        //блокируем блок настроек MySQL и включаем блок выбора файла DBF
        ui_groupSQL->setEnabled(false);
        ui_groupDBF->setEnabled(true);

        //читаем имя файла DBF-базы данных
        //m_settings.beginGroup("/Settings/dbf");
        this->dbf->fileName = m_settings.value("/Settings/dbf/filename", "").toString();
        //m_settings.endGroup();
        qDebug() << "we read DBF settings. File name " << this->dbf->fileName;

        if (dbf->fileName != "") {
            //если имя файла не пусто - продолжаем
            my_informer->set_tb_name(dbf->fileName);
            ui_labelDBFname->setText(dbf->fileName);
            this->opisateli.clear();
            opisateli = my_informer->tb_describe("DBF");
            qDebug() << "we read DBF settings. Count opisateli " << opisateli.count();

            //вызываем функцию выбора и установки описателей
            this->set_opisateli_from_settings();
            this->update_ui_db_controls();

        } else {
            //если получено пустое имя файла -
            //заблокировать дальнейшие контролы
            //и установить фокус ввода на выборе имени файла
            //ui_gridLayout_2;
            qDebug() << "we read DBF settings. File is empty";

            return;
        }


    }

    //qDebug() << "Closing group settings. Count " << i;

    //Читаем настройки фильтра
    bool filter = m_settings.value("/Settings/filterIsOn", "false").toBool();
    if (filter) {
        //если фильтр включен - включаем контрол в UI и читаем настройки
        ui_filterBox->setChecked(true);
        this->read_filter_settings();
    } else {
        //если в конфиге фильтр выключен - выключем его в UI на всякий случай
        ui_filterBox->setChecked(false);
    }

    //читаем номер текущей вкладки (по умолчанию - вкладка с источником данных)
    int ind = m_settings.value("/Settings/tabIndex", "3").toInt();
    if (ind > ui_tabWidget->count()-1) {
        ind = 0;
    }
    ui_tabWidget->setCurrentIndex(ind);


}

void cengen::read_filter_settings() {
    //читаем название файла фильтра и обновляем UI по необходимости
    filterDbf.fileName = m_settings.value("/Settings/filter/FileName", "").toString();
    if (filterDbf.fileName == "") {
        this->file_is_ready = false;
        return;
    }

    on_filterFileName_changed();

    ui_filterWhatBox->setCurrentIndex(m_settings.value("/Settings/filter/WhatBox", "0").toInt());
    ui_filterWhereBox->setCurrentIndex(m_settings.value("/Settings/filter/WhereBox", "0").toInt());

    ui_filterMethodBox->setCurrentIndex(m_settings.value("/Settings/filter/Method", "0").toInt());

    ui_filterLineText->setText(m_settings.value("/Settings/filter/LineText", "").toString());

    ui_filterWhatToFoundBox->setCurrentIndex(m_settings.value("/Settings/filter/WhatToFoundBox", "0").toInt());


}

void cengen::on_connectMySQLButton_clicked()
{
    //подготавливаем соединение с базой данных
    sql = new SqlConfig;
    sql->dbHost = ui_lineHost->text();
    sql->dbName = ui_lineName->text();
    sql->dbUser = ui_lineUser->text();
    sql->dbPass = ui_linePass->text();
    qDebug() << "prepared";

    if (my_informer->prepare(sql)) {
        update_ui_connection_established();
        //my_informer->tb_describe("test");
    }
}

void cengen::on_comboTbList_activated(QString tbName)
{

    QStringList list = my_informer->tb_describe(tbName);
    this->update_ui_tb_fields(list);

    my_informer->set_tb_name(tbName);
    ui_save_db_config_button->setEnabled(true);
    this->sql->tbName = tbName;
    this->db_is_ready = true;


}
void cengen::update_ui_tb_fields(QStringList list) {
    //обновляем пользовательские контролы для выбора в таблице цены, названия и т.п.
    ui_comboTnomer->clear();
    ui_comboTname->clear();
    ui_comboTbarcode->clear();
    ui_comboTprice->clear();

    ui_comboTnomer->addItems(list);
    ui_comboTname->addItems(list);
    ui_comboTbarcode->addItems(list);
    ui_comboTprice->addItems(list);

    ui_save_db_config_button->setEnabled(true);
}

void cengen::on_save_db_config_button_clicked()
{
    //сохраняем, если возможно, полученные настройки для подключения к базе

    config.tnomer = ui_comboTnomer->currentText();
    config.tname = ui_comboTname->currentText();
    config.tbarcode = ui_comboTbarcode->currentText();
    config.tprice = ui_comboTprice->currentText();

    config_index[0] = ui_comboTnomer->currentIndex();
    config_index[1] = ui_comboTname->currentIndex();
    config_index[2] = ui_comboTbarcode->currentIndex();
    config_index[3] = ui_comboTprice->currentIndex();

    my_informer->set_fields(&config);
    this->describer_is_ready = true;

    qDebug() << "Save db headers. 3 is " << config_index[3];
}

void cengen::update_ui_db_controls() {


    if (db_is_ready && this->db_source) {
        ui_lineHost->setText(sql->dbHost);
        ui_lineName->setText(sql->dbName);
        ui_lineUser->setText(sql->dbUser);
        ui_linePass->setText(sql->dbPass);
        this->update_ui_connection_established();

    }

    if (this->describer_is_ready) {
        if (this->db_source) {
            this->on_comboTbList_activated(sql->tbName);
        }
        ui_comboTnomer->blockSignals(true);
        ui_comboTname->blockSignals(true);
        ui_comboTbarcode->blockSignals(true);
        ui_comboTprice->blockSignals(true);

        this->ui_comboTnomer->setCurrentIndex(config_index[0]);
        this->ui_comboTname->setCurrentIndex(config_index[1]);
        this->ui_comboTbarcode->setCurrentIndex(config_index[2]);
        this->ui_comboTprice->setCurrentIndex(config_index[3]);

        ui_comboTnomer->blockSignals(false);
        ui_comboTname->blockSignals(false);
        ui_comboTbarcode->blockSignals(false);
        ui_comboTprice->blockSignals(false);

        this->on_save_db_config_button_clicked();
        ui_save_db_config_button->setEnabled(true);
    }
//qDebug() << "updating";
}

void cengen::update_ui_connection_established() {
    QStringList tables = my_informer->db_describe();
    ui_comboTbList->clear();
    ui_comboTbList->addItems(tables);
    ui_comboTbList->setEnabled(true);

}

void cengen::on_radioButton_1_clicked()
{
    this->method = "tbarcode";
    ui_lineEdit->setValidator(ui_bvalidator);
    this->new_line_ready();
}

void cengen::on_radioButton_2_clicked()
{
    this->method = "tnomer";
    ui_lineEdit->setValidator(ui_nvalidator);
    this->new_line_ready();
}

void cengen::on_radioButton_3_clicked()
{
    this->method = "tname";
    ui_lineEdit->setValidator(ui_svalidator);
    this->new_line_ready();
}

void cengen::on_radioButton_5_clicked()
{
    this->method = "any";
    ui_lineEdit->setValidator(ui_svalidator);
    this->new_line_ready();
}

void cengen::delete_line_from_table(int pos) {
    qDebug() << "pos" << pos;
    ui_tableWidget->removeRow(pos);
    int num = ui_tableWidget->rowCount();
    for (int i = 0; i<num; i++) {
        //int nomer = ui_tableWidget->item(i, 0)->text().toInt();
        ui_tableWidget->item(i, 0)->setText(QString::number(i+1));        
    }
    ui_countLabel->setText(QString::number(num));

    //тут бы еще сами кнопки перенумеровать


    this->new_line_ready();
}

void cengen::on_radioButton_clicked()
{
    //устанавливаем метод поиска через сетевую MySQL-базу данных
    this->db_source = 1;
    //qDebug() << "selected SQL";
    m_settings.sync();
    m_settings.beginGroup("/Settings");
    m_settings.setValue("/source", 1);
    m_settings.endGroup();
    m_settings.sync();
    this->readSettings();

    qDebug() << "set read sql";
    ui_radioButton_3->setEnabled(true);
    //db_is_ready = my_informer->prepare(sql);
    //qDebug() << "set read sql 777";
    ui_groupDBF->setEnabled(false);
    ui_groupSQL->setEnabled(true);
}

void cengen::on_radioButton_4_clicked()
{
    //устанавливаем метод поиска товара через чтение локального DBF-файла
    this->db_source = 0;
    m_settings.beginGroup("/Settings");
    m_settings.setValue("/source", 0);
    m_settings.endGroup();
    this->readSettings();

    //ui_radioButton_3->setEnabled(false);
    ui_groupDBF->setEnabled(true);
    ui_groupSQL->setEnabled(false);
}

void cengen::on_tableWidget_cellClicked(int row, int column)
{
    //qDebug() << "clicked row "<< row << "; col " << column;
    if (column == 6) {
        this->delete_line_from_table(row);
    }

}

void cengen::on_tableWidget_itemClicked(QTableWidgetItem* item)
{

}

void cengen::on_pushButton_5_clicked()
{
    //выбор DBF-файла
    QString str = QFileDialog::getOpenFileName(0, "Open Dialog", "", "DBF-files (*.dbf)");
    if (str == "") return;
    ui_labelDBFname->setText(str);
    //this->dbf = new DbfConfig;
    dbf->fileName = str;
    my_informer->set_tb_name(str);
    QStringList list = my_informer->tb_describe(str);
    this->update_ui_tb_fields(list);

}

bool cengen::read_opisateli_from_settings() {
    //m_settings.beginGroup("/Settings/describer");
    config_index[0] = m_settings.value("/Settings/describer/tnomer", -1).toInt();
    config_index[1] = m_settings.value("/Settings/describer/tname", -1).toInt();
    config_index[2] = m_settings.value("/Settings/describer/tbarcode", -1).toInt();
    config_index[3] = m_settings.value("/Settings/describer/tprice", -1).toInt();
    //m_settings.endGroup();
    if (config_index[0] == -1 ||
            config_index[1] == -1 ||
            config_index[2] == -1 ||
            config_index[3] == -1
            ) {
        return false;
    }
    return true;


}

void cengen::set_opisateli_from_settings()
{
    if (!this->db_source && !opisateli.count()) {
        //если в списке ноль полей - выбираем файл
        qDebug() << "we read DBF settings. Zero file";

        return;
    }
    this->db_is_ready = true;
    //вызываем функцию установки описателей на пользователький интерфейс
    update_ui_tb_fields(opisateli);

    //читаем настройки, что где в этой базе данных
    describer_is_ready = read_opisateli_from_settings();
    if (describer_is_ready) {
        //qDebug() << "we read DBF settings. Set fields";

        config.tnomer = opisateli.at(config_index[0]);
        config.tname = opisateli.at(config_index[1]);
        config.tbarcode = opisateli.at(config_index[2]);
        config.tprice = opisateli.at(config_index[3]);
        my_informer->set_fields(&config);
        return;

    }
}

void cengen::set_tableWidget_header(QTableWidget *table) {

    table->horizontalHeader()->resizeSection(0, 25);
    table->horizontalHeader()->resizeSection(1, 380);
    table->horizontalHeader()->resizeSection(2, 120);
    table->horizontalHeader()->resizeSection(3, 80);
    table->horizontalHeader()->resizeSection(4, 70);
    table->horizontalHeader()->resizeSection(5, 70);
    table->horizontalHeader()->resizeSection(6, 30);

    table->verticalHeader()->hide();

}

void cengen::add_table_item(QTableWidget *table, int position, Tovar tovar) {


    QTableWidgetItem* itemN = new QTableWidgetItem(QString::number(position + 1));
    QTableWidgetItem* itemName = new QTableWidgetItem(tovar.name_of_tovar);
    QTableWidgetItem* itemBarcode = new QTableWidgetItem(tovar.barcode);
    QTableWidgetItem* itemNomer = new QTableWidgetItem(QString::number(tovar.nomer_of_tovar));
    QTableWidgetItem* itemPrice1 = new QTableWidgetItem(QString::number(tovar.price1));
    QTableWidgetItem* itemPrice2 = new QTableWidgetItem(QString::number(tovar.price2));

    table->setItem(position, 0, itemN);
    table->setItem(position, 1, itemName);
    table->setItem(position, 2, itemBarcode);
    table->setItem(position, 3, itemNomer);
    table->setItem(position, 4, itemPrice1);
    table->setItem(position, 5, itemPrice2);


    /*
    ButtonForTable* del_button = new ButtonForTable;
    del_button->setPosition(position);
    del_button->setText("del");
    del_button->setToolTip("DELETE FROM TABLE");
    connect(del_button, SIGNAL(clickedID(int)), SLOT(delete_line_from_table(int)));
    */
//        QLineEdit* brLine = new QLineEdit;
//        brLine->setText(tovar.barcode);
//        brLine->setValidator(ui_bvalidator);
//        ui_tableWidget->setCellWidget(position, 2, brLine);
//        qDebug() << "text " << ui_tableWidget->item(position,2)->text();





    //ui_tableWidget->setCellWidget(position, 6, del_button);

    /*
    QPushButton* button = new QPushButton("del");
    //QTableWidgetItem* item2 = new QTableWidgetItem();
    ui_tableWidget->setItem(0, 1, item);
    ui_tableWidget->setCellWidget(0, 0, button);
    */
}

void cengen::on_spinLimit_valueChanged(int number)
{
    qDebug() << "limit = " << number;
    my_informer->set_limit_search(number);
}

void cengen::on_maxButton_clicked()
{
    //нажата кнопка выбора лимита поиска без ограничений
    ui_spinLimit->setValue(0);
}

/*
void cengen::on_tableWidget_cellEntered(int row, int column)
{
//избавиться от этой функции
}
*/

void cengen::on_tableWidget_cellChanged(int row, int column)
{
    if (editing_price2) {
        editing_price2 = false;
        new_line_ready();
        return;
    }

    if (add_flag) {
        return;
    }

    if (row < ui_tableWidget->rowCount()-1) {
        row++;
    } else {
        if (column < ui_tableWidget->columnCount() -1) {
            column++;
        }
    }

    QTableWidgetItem* item = ui_tableWidget->item(row, column);
    ui_tableWidget->setCurrentItem(item);


}

void cengen::on_tableWidget_itemEntered(QTableWidgetItem* item)
{
    int row = item->row();
    qDebug() << "row entered = " << row;
    if (row<ui_tableWidget->rowCount()) {

        qDebug() << "set Focus";
    }

}

void cengen::on_show_editor_button_clicked()
{
    if (file_is_ready) {

        shablonElement = domDoc.documentElement();
        shablon_editor->load_xml_data_into_editor(&shablonElement);
        shablon_editor->set_file_name(this->file.fileName());
        shablon_editor->showFullScreen();


        shablon_editor->show();
    }

}

void cengen::on_action_activated()
{
    //Сохранение списка товаров
    qDebug() << "saving tovar list";

    QList<Tovar> spisok= get_tovar_list(ui_tableWidget, "x");

    QDomDocument doc;

    doc = this->convert_tovar_list_into_xml(spisok);


    //записываем все это дело в файл
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save tovar list"),"" , tr("Tovar lists (*.tov)"));
    if (fileName != "") {
        QFile file(fileName);
        if(file.open(QIODevice::WriteOnly)) {
            QString shablon_array = doc.toString();
            QTextStream out(&file);
            //out.setCodec(codec_utf8);
            out << shablon_array;

            file.close();
        }
        //qDebug() << "template:: \n " << doc.toString();

    }


}

void cengen::on_action_2_triggered()
{
    qDebug() << "Load tovar list selected";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open tovar list"), "", tr("Tovar lists (*.tov)"));
    if (fileName == "") {
        qDebug() << "Please select file name";
        return;
    }

    QDomDocument doc;
    //QDomElement domElement;

    QFile file;
    file.setFileName(fileName);

    if (file.open(QIODevice::ReadOnly)) {
        if (doc.setContent(&file)) {
            QList<Tovar> tovarList;
            tovarList = convert_xml_into_tovar_list(doc);
            this->load_tovar_list_into_cengen(tovarList);

        }
    } else {
        //error что файл не открывается
        qDebug() << "cant open file";
        return;
    }

}

QList<Tovar> cengen::convert_xml_into_tovar_list(QDomDocument doc) {

    QDomElement domElement = doc.documentElement();
    QList<Tovar> tovarList;

    //раз файл открылся - переводим xml в TovarList

    QDomNode node = domElement.firstChild();
    while (!node.isNull()){
        if (node.isElement()) {
            QDomElement item = node.toElement();
            Tovar tovarItem;

            if (item.tagName() == "tovarItem") {
                QDomNode nodeItem = node.firstChild();
                while (!nodeItem.isNull()) {
                    if (nodeItem.isElement()) {
                        QDomElement elementItem = nodeItem.toElement();
                        if (!elementItem.isNull()) {
                            QString itemName = elementItem.tagName();
                            QString itemValue = elementItem.text();
                            if (itemName == "name") {
                                tovarItem.name_of_tovar = itemValue;
                            }
                            if (itemName == "barcode") {
                                tovarItem.barcode = itemValue;
                            }
                            if (itemName == "price1") {
                                tovarItem.price1 = itemValue.toFloat();
                            }
                            if (itemName == "price2") {
                                tovarItem.price2 = itemValue.toFloat();
                            }
                            if (itemName == "tnomer") {
                                tovarItem.nomer_of_tovar = itemValue.toInt();
                            }
                        }
                    }
                    nodeItem = nodeItem.nextSibling();
                }

                tovarList << tovarItem;
            }

        }
        node = node.nextSibling();
    }
    return tovarList;

}

QDomDocument cengen::convert_tovar_list_into_xml(QList<Tovar> spisok) {
    QDomDocument doc;

    QDomNode xmlNode = doc.createProcessingInstruction("xml",
                                    "version=\"1.0\" encoding=\"UTF-8\"");
    doc.insertBefore(xmlNode, doc.firstChild());
    //doc.createElement("tovarList");

    QDomElement domList = doc.createElement("tovarList");

    for (int i = 0; i<spisok.count(); i++) {
        QDomElement tovarElement = doc.createElement("tovarItem");

        QDomElement nameElement = doc.createElement("name");
        QDomText dm1 = doc.createTextNode(spisok.at(i).name_of_tovar);
        nameElement.appendChild(dm1);
        tovarElement.appendChild(nameElement);

        QDomElement priceElement = doc.createElement("price1");
        QDomText dm2 = doc.createTextNode(QString::number(spisok.at(i).price1));
        priceElement.appendChild(dm2);
        tovarElement.appendChild(priceElement);

        QDomElement price2Element = doc.createElement("price2");
        QDomText dm3 = doc.createTextNode(QString::number(spisok.at(i).price2));
        price2Element.appendChild(dm3);
        tovarElement.appendChild(price2Element);

        QDomElement tnomerElement = doc.createElement("tnomer");
        QDomText dm4 = doc.createTextNode(QString::number(spisok.at(i).nomer_of_tovar));
        tnomerElement.appendChild(dm4);
        tovarElement.appendChild(tnomerElement);

        QDomElement barcodeElement = doc.createElement("barcode");
        QDomText dm5 = doc.createTextNode(spisok.at(i).barcode);
        barcodeElement.appendChild(dm5);
        tovarElement.appendChild(barcodeElement);

        domList.appendChild(tovarElement);
    }

    doc.appendChild(domList);
    return doc;
}

void cengen::on_action_3_activated()
{
    qDebug() << "new spisok selected";

    ui_tableWidget->setRowCount(0);

}

void cengen::on_action_8_activated()
{
    //функция вычитания списка
    //из существующего списка убираем позиции, которые есть в загружаемом списке
    //сверку ведем по товарному номеру

    qDebug() << "Tovar list minus";

    QString fileName = QFileDialog::getOpenFileName(this, tr("Select tovar list for minus"), "", tr("Tovar lists (*.tov)"));
    if (fileName == "") {
        qDebug() << "Please select file name";
        return;
    }

    QDomDocument doc;

    QFile file;
    file.setFileName(fileName);

    if (file.open(QIODevice::ReadOnly)) {
        if (doc.setContent(&file)) {
            QList<Tovar> newTovarList, oldTovarList, finalList;
            newTovarList = convert_xml_into_tovar_list(doc);
            oldTovarList = get_tovar_list(ui_tableWidget, "x");

            finalList = minus(oldTovarList, newTovarList);

            ui_tableWidget->setRowCount(0);
            this->load_tovar_list_into_cengen(finalList);

        }
    } else {
        //error что файл не открывается
        qDebug() << "cant open file";
        return;
    }
}

QList<Tovar> cengen::minus(QList<Tovar> oldList, QList<Tovar> newList) {
    QList<Tovar> final;

    final = oldList;
    int k = 0;

    for (int i = 0; i<newList.count(); i++) {

        for (int j = 0; j<oldList.count(); j++) {
            if (newList.at(i).nomer_of_tovar == oldList.at(j).nomer_of_tovar) {
                final.removeAt(j-k);
                qDebug() << "j and k = " << j << k;
                k++;
            }
        }

    }

    return final;
}
/*
void cengen::on_checkBox_toggled(bool checked)
{
    ui_filterGrid->setEnabled(checked);
}
*/

void cengen::on_filterBox_toggled(bool arg1)
{
    filter_is_on = arg1;
    if (arg1) {
        qDebug() << "Enabled filter";
        this->read_filter_settings();

    }

}

void cengen::on_filterFileSelectButton_clicked()
{
    //выбор DBF-файла
    QString str = QFileDialog::getOpenFileName(0, "Select FILTER file", "", "DBF-files (*.dbf)");
    if (str == "") return;

    filterDbf.fileName = str;

    on_filterFileName_changed();

}

void cengen::on_filterFileName_changed() {
    //изменилось имя файла для фильтра - подготовливаем Tinformer и обновляем UI
    filterInformer->prepare(&filterDbf);
    filterInformer->set_tb_name(filterDbf.fileName);
    QStringList list = filterInformer->tb_describe(filterDbf.fileName);

    ui_filterWhatBox->clear();
    ui_filterWhatBox->addItems(list);

    ui_filterWhereBox->clear();
    ui_filterWhereBox->addItems(list);
}

QList<Tovar> cengen::apply_filter(QList<Tovar> inputList) {
    QList<Tovar> filteredList;


    filterConfig.tnomer = ui_filterWhereBox->currentText();
    filterConfig.tprice = ui_filterWhereBox->currentText();
    filterConfig.tname = ui_filterWhereBox->currentText();
    filterConfig.tbarcode = ui_filterWhereBox->currentText();

    int WhatToFound = ui_filterWhatToFoundBox->currentIndex();
    qDebug() << "What to found is " << WhatToFound;
    switch (WhatToFound) {
    case 0:
        filterConfig.tnomer = ui_filterWhatBox->currentText();
        break;
    case 1:
        filterConfig.tbarcode = ui_filterWhatBox->currentText();
        break;
    case 2:
        filterConfig.tname = ui_filterWhatBox->currentText();
        break;
    case 3:
        filterConfig.tprice = ui_filterWhatBox->currentText();
        break;

    }

    //filterConfig.tbarcode = ui_filterWhatBox->currentText();


    filterInformer->set_fields(&filterConfig);

    filterInformer->set_limit_search(1);

    int method = ui_filterMethodBox->currentIndex();
    QString search = ui_filterLineText->text();
    bool itemfound;

    qDebug() << "Method in filter " << method;

    Tovar tovarItem, tovarFound;

    for (int i = 0; i<inputList.count(); i++) {
        tovarItem = inputList.at(i);
        //qDebug() << "filter i=" << i;

        QString searchText, searchMethod;
        //в зависимости от выбранного ищем по номеру, цене и т.п.
        switch (WhatToFound) {
        case 0:
            searchText = QString::number(tovarItem.nomer_of_tovar);
            searchMethod = "tnomer";
            break;
        case 1:
            searchText = tovarItem.barcode;
            searchMethod = "tbarcode";
            break;
        case 2:
            searchText = tovarItem.name_of_tovar;
            searchMethod = "tname";
            break;
        case 3:
            searchText = QString::number(tovarItem.price1);
            searchMethod = "tprice";
        default:
            searchText = tovarItem.nomer_of_tovar;
            searchMethod = "tnomer";
            break;
        }
        //qDebug() << "Search text in filter: " << searchText;

        QList<Tovar> foundList = filterInformer->info(searchText, searchMethod);
        if (foundList.count()) {
            itemfound = false;
            tovarFound = foundList.at(0);
            //qDebug() << "tovar is " << tovarFound.price1;
            qDebug() << "FOUND in filter: " << tovarFound.name_of_tovar;

            //проверяем, используя выбранный метод сравнения
            switch (method) {
            case 0:
                if (tovarFound.price1 == search.toFloat()) itemfound = true;
                break;
            case 1:
                if (tovarFound.price1 != search.toFloat()) itemfound = true;
                break;
            case 2:
                if (tovarFound.price1 > search.toFloat()) itemfound = true;
                break;
            case 3:
                if (tovarFound.price1 < search.toFloat()) itemfound = true;
                break;
            case 4:
                if (tovarFound.name_of_tovar.contains(searchText, Qt::CaseInsensitive)) itemfound = true;
            default:
                break;
            }

            if (itemfound) filteredList <<tovarItem;

        }
    }



    return filteredList;
}


void cengen::on_radioButton_6_clicked()
{
    this->paperOrientation = "portrate";
    update_values();
}

void cengen::on_radioButton_7_clicked()
{
    this->paperOrientation = "landscape";
    update_values();
}

void cengen::on_action_4_activated()
{
    this->on_exitButton_clicked();
}

void cengen::on_comboTnomer_currentIndexChanged(int index)
{
    //при изменении комбобоксов из списка соответствий - сохраняем
    on_save_db_config_button_clicked();
}

void cengen::on_comboTname_currentIndexChanged(int index)
{
    on_save_db_config_button_clicked();
}

void cengen::on_comboTbarcode_currentIndexChanged(int index)
{
    on_save_db_config_button_clicked();
}

void cengen::on_comboTprice_currentIndexChanged(int index)
{
    on_save_db_config_button_clicked();
}
