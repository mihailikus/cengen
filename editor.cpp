#include "editor.h"
#include "ui_editor.h"

editor::editor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::editor)
{
    ui->setupUi(this);

    line_number = 0;

    codec_utf8 = QTextCodec::codecForName("UTF-8");

    ui_view = qFindChild<QGraphicsView*>(this, "view");
    ui_scene = new QGraphicsScene;
    ui_view->setScene(ui_scene);
    connect(ui_scene, SIGNAL(selectionChanged()), SLOT(on_scene_selected()));

    ui_preView = qFindChild<QGraphicsView*>(this, "preView");
    ui_preScene = new QGraphicsScene;
    ui_preView->setScene(ui_preScene);
    ui_preView->scale(0.2, 0.2);


    ui_propertBox = qFindChild<QGroupBox*>(this, "propertBox");
    ui_propertBox->setVisible(false);

    ui_shalon_nameEdit = qFindChild<QLineEdit*>(this, "shablon_nameEdit");
    ui_textEdit = qFindChild<QLineEdit*>(this, "textEdit");

    ui_nameButton = qFindChild<QPushButton*>(this,"nameButton");


    ui_widthSpin = qFindChild<QSpinBox*>(this, "widthSpin");
    ui_heithSpin = qFindChild<QSpinBox*>(this, "heithSpin");

    ui_fontSizeSpin = qFindChild<QSpinBox*>(this, "fontSizeSpin");

    //спинбоксы для управления свойствами выделенного объекта
    ui_pwidthSpin = qFindChild<QSpinBox*>(this, "pwidthSpin");
    ui_pheithSpin = qFindChild<QSpinBox*>(this, "pheithSpin");
    ui_pwidthSpin->setMaximum(2000);
    ui_pheithSpin->setMaximum(2000);
    ui_fontSizeSpin->setMaximum(500);
    ui_pwidthSpin->setMinimum(1);
    ui_pheithSpin->setMinimum(1);
    ui_fontSizeSpin->setMinimum(0);
    connect (ui_pwidthSpin, SIGNAL(valueChanged(int)), SLOT(on_propert_spin_changed()));
    connect (ui_pheithSpin, SIGNAL(valueChanged(int)), SLOT(on_propert_spin_changed()));
    connect (ui_fontSizeSpin, SIGNAL(valueChanged(int)), SLOT(on_propert_spin_changed()));

    c_items_initialize();
    ui_view->scale(0.5, 0.5);

    fileName = "template.cen";


}

editor::~editor()
{
    delete ui;
}

void editor::on_exitButton_clicked()
{
    if (this->parent()) {
        on_previewButton_clicked();
        //qDebug() << doc.toString() << "\n----------------------------\n";

        //int length = doc.toString().length();
        emit shablon_is_ready(doc);
    }

    close();
}

void editor::on_clearButton_clicked()
{
    ui_scene->clear();
    c_items_initialize();
    //buttons_initialize();
}

void editor::delete_item(QGraphicsItem *item) {
    ui_scene->removeItem(item);
}

void editor::on_saveButton_clicked()
{
    //создаем превью (вместе с XML-документом)
    this->on_previewButton_clicked();

    //записываем все это дело в файл
    fileName = QFileDialog::getSaveFileName(this, tr("Save template"), fileName, tr("Cennic templates (*.cen)"));
    qDebug() << "file name is " << fileName;
    if (fileName != "") {
        QFile file(fileName);
        if(file.open(QIODevice::WriteOnly)) {
            QString shablon_array = doc.toString();
            QTextStream out(&file);
            out.setCodec(codec_utf8);
            out << shablon_array;

            file.close();
        }
        //qDebug() << "template:: \n " << doc.toString();

    }



}

void editor::on_previewButton_clicked()
{
    ui_preView->setVisible(true);
    //создаем XML-документ
    doc.clear();
    QDomNode xmlNode = doc.createProcessingInstruction("xml",
                                    "version=\"1.0\" encoding=\"UTF-8\"");
    doc.insertBefore(xmlNode, doc.firstChild());
    //doc.createElement("cennic");

    QDomElement cennic = doc.createElement("cennic");

    //устанавливаем название шаблона
    QDomElement elementName = doc.createElement("name");
    QDomText domText = doc.createTextNode(ui_shalon_nameEdit->text());
    elementName.appendChild(domText);
    cennic.appendChild(elementName);

    //устанавливаем базовый размер - ширину и высоту
    QDomElement domElement = doc.createElement("base");
    //QDomAttr domAttr = doc.createAttribute("width");
    //domAttr.setValue(QString::number(ui_widthSpin->value()));
    domElement.setAttribute("width", QString::number(ui_widthSpin->value()));
    domElement.setAttribute("heith", QString::number(ui_heithSpin->value()));
    //doc.appendChild(domElement);
    cennic.appendChild(domElement);


    foreach(QGraphicsItem* item, c_items) {
        //qDebug() << "Item type is " << item->acceptDrops()
        QString value = c_items.key(item);
        QString item_value = value;
        QGraphicsItem* item = c_items[value];
        if (item->scene() == ui_scene) {
            QDomElement elem;
            if (value.contains("line")) {
                value = "line";
            }

            //устанавливаем атрибуты для поля названия товара
            if (value == "barcode" || value == "good" || value == "line") {
                elem = doc.createElement(value);
            } else {
                elem = doc.createElement("text");
                QString method;
                if (value.contains("text") || value == "date") {
                    if (value == "date") {
                        method = "date";
                    } else {
                        method = "textInBox";
                    }
                    QDomText dm = doc.createTextNode(c_text_items[item_value].text);
                    elem.appendChild(dm);
                } else {
                    method = value;
                }
                elem.setAttribute("method", method);

            }

            if (value == "price" || value == "priceOld") {
                //устанавливаем дополнительные параметры для рублей и копеек
                QStringList rub_kop = c_text_items[item_value].text.split("::");
                if (rub_kop.count()<2) {
                    rub_kop << "kop";
                }
                elem.setAttribute("rub", rub_kop.at(0));
                elem.setAttribute("kop", rub_kop.at(1));

            }

            QString startX, startY, width, heith;
            startX = QString::number(item->x());
            startY = QString::number(item->y());
            width = QString::number(item->sceneBoundingRect().width());
            heith = QString::number(item->sceneBoundingRect().height());

            if (value == "line") {
                //вычисляем параметры линии
                float rotation = c_text_items[item_value].text.toFloat();
                item->rotate(0-rotation);
                float linethick = item->sceneBoundingRect().height();
                float len = item->sceneBoundingRect().width();
                width = QString::number(len * cos(rotation/180*M_PI));
                heith = QString::number(len * sin(rotation/180*M_PI));
                item->rotate(rotation);
                elem.setAttribute("linethick", linethick);

            } else {
                //если не линия - записываем в XML параметры шрифта
                QString font_family = c_text_items[value].font.family();
                elem.setAttribute("font-family", font_family);
                int font_size = c_text_items[value].font.pointSize();
                elem.setAttribute("font-size", QString::number(font_size));
            }



            elem.setAttribute("startX", startX);
            elem.setAttribute("startY", startY);
            elem.setAttribute("width", width);
            elem.setAttribute("heith", heith);

            //qDebug() << "LINE startX etc " << startX << startY << width << heith;


            cennic.appendChild(elem);
        }
    }
    doc.appendChild(cennic);


    ui_preScene->clear();
    Tovar tovar;
    tovar.barcode = c_text_items["barcode"].text;
    tovar.name_of_tovar = c_text_items["good"].text;
    tovar.nomer_of_tovar = c_text_items["nomer"].text.toInt();
    tovar.price1 = 299.90;
    tovar.price2 = 999.90;
    Cennic* preview_cennic = new Cennic(&tovar, doc.documentElement());
    QPoint* pos = preview_cennic->render(ui_preScene, 0, 0);

    ui_preView->fitInView(0, 0, pos->x(), pos->y(), Qt::KeepAspectRatio);


}

void editor::on_widthSpin_valueChanged(int value)
{
    ui_scene->removeItem(baseRect);
    baseRect = ui_scene->addRect(0, 0, value, ui_heithSpin->value());

}

void editor::on_heithSpin_valueChanged(int value)
{
    ui_scene->removeItem(baseRect);
    baseRect = ui_scene->addRect(0, 0, ui_widthSpin->value(), value);

}

void editor::set_spin_value(QSpinBox *widget, float value) {
    widget->blockSignals(true);
    widget->setValue(value);
    widget->blockSignals(false);
}

void editor::on_scene_selected() {
    QList<QGraphicsItem* > items = ui_scene->selectedItems();
    if (items.count() == 1) {
        ui_propertBox->setVisible(true);
        //qDebug() << "Selected 1 object";
        QGraphicsItem* item = items.at(0);
        qreal width, heith;
        QString text = c_items.key(item);
        if (text.contains("line")) {
            float angle = c_text_items[text].text.toFloat();
            item->rotate(0-angle);
            width = item->sceneBoundingRect().width();
            heith = item->sceneBoundingRect().height();
            if (angle<0) {
                heith = 0-heith;
            }
            item->rotate(angle);


        } else {
            width = item->sceneBoundingRect().width();
            heith = item->sceneBoundingRect().height();
        }

        //устанавливаем размеры прямоугольника
        set_spin_value(ui_pwidthSpin, width);
        set_spin_value(ui_pheithSpin, heith);

        //устанавливаем размер шрифта в поле для его изменения
        QFont font = c_text_items[text].font;
        qDebug() << "FONT is " << font.pointSize();
        set_spin_value(ui_fontSizeSpin, font.pointSize());


        //устанавливаем текст
        QString value = c_items.key(item);
        ui_textEdit->blockSignals(true);
        ui_textEdit->setText(c_text_items[value].text);
        ui_textEdit->blockSignals(false);


    } else {
        ui_propertBox->setVisible(false);
    }
}

void editor::on_propert_spin_changed() {
    QGraphicsItem* item = ui_scene->selectedItems().at(0);
    QString text = c_items.key(item);
    //qDebug() << "TEXT in spin is " << text;
    float angle;

    if (text.contains("line")) {
        angle = c_text_items[text].text.toFloat();
        item->rotate(0-angle);
        float len = ui_pwidthSpin->value();
        float linethick = ui_pheithSpin->value();
        QString api_changed;
        if ( (linethick - item->sceneBoundingRect().height()) < 0.01) {
            api_changed = "width";
        } else {
            api_changed = "heith";
        }
        float startX = item->pos().x();
        float startY = item->pos().y();
        float width = len * cos (angle/180*M_PI);
        float heith = len * sin (angle/180*M_PI);
        ui_scene->removeItem(item);
        add_element_to_scene(text, startX, startY, width, heith, linethick);
        item = c_items.value(text);
        item->setSelected(true);

        if (api_changed == "width") {
            ui_pwidthSpin->setFocus();
        }
        if (api_changed == "heith") {
            ui_pheithSpin->setFocus();
        }


    } else {

        qreal width = ui_pwidthSpin->value() / item->sceneBoundingRect().width();
        qreal heith = ui_pheithSpin->value() / item->sceneBoundingRect().height();

        item->scale(width, heith);

        c_text_items[text].font.setPointSize(ui_fontSizeSpin->value());



    }

    on_previewButton_clicked();

}

void editor::add_element_to_scene(QString type, float startX, float startY, float width, float heith, float linethick) {
    this->add_element_to_scene(type, startX, startY, width, heith, QBrush(Qt::red), QFont("arial", 10), QString::number(linethick), false);
}

void editor::c_items_initialize() {
    c_items.clear();
    c_text_items.clear();

    //базовый размер
    baseRect = ui_scene->addRect(0, 0, ui_widthSpin->value(), ui_heithSpin->value());

    //добавляем основные элементы ценника - название товара, штрих-код, цену и т.п.
    add_element_to_scene("barcode", 100, 200, 300, 70, QBrush(Qt::darkBlue), QFont("Arial", 7), "216975");

    add_element_to_scene("good", 0, 0, ui_widthSpin->value(), 230, QBrush(Qt::green), QFont("Arial", 43), "3441 small paper best clean (200 ml.)");

    add_element_to_scene("nomer", 0, 0, 200, 20, QBrush(Qt::blue), QFont("Arial", 20), "74124");

    add_element_to_scene("price", 0, 0, 300, 70, QBrush(Qt::red), QFont("Arial", 50), "r::k");

    add_element_to_scene("priceOld", 0, 0, 300, 70, QBrush(Qt::lightGray), QFont("Arial", 50), "rub::kop");

    add_element_to_scene("date", 0, 0, 200, 20, QBrush(Qt::magenta), QFont("Arial", 20), "dd.MM.yy");

    add_element_to_scene("barcodeAsText", 100, 200, 200, 20, QBrush(Qt::darkYellow), QFont("Arial", 20), "4600702003840");

}

void editor::switch_cennic_element(QGraphicsItem *item) {
    if (item->scene() == ui_scene) {
        ui_scene->removeItem(item);
        //ui_nameButton->setText(tr("ADD NAME"));
    } else {
        ui_scene->addItem(item);
        //ui_nameButton->setText(tr("DEL NAME"));
    }
}

void editor::on_nameButton_clicked()
{
    switch_cennic_element(c_items["good"]);

}

void editor::on_priceButton_clicked()
{
    switch_cennic_element(c_items["price"]);

}

void editor::on_barcodeButton_clicked()
{
    switch_cennic_element(c_items["barcode"]);
}

void editor::on_oldpriceButton_clicked()
{
    switch_cennic_element(c_items["priceOld"]);
}

void editor::on_dateButton_clicked()
{
    switch_cennic_element(c_items["date"]);
}

void editor::on_nomerButton_clicked()
{
    switch_cennic_element(c_items["nomer"]);
}

void editor::on_fontSelectButton_clicked()
{
    bool ok;
    QString value = c_items.key(ui_scene->selectedItems().at(0));
    QFont font = c_text_items[value].font;
    QFont fnt = QFontDialog::getFont(&ok, font);
    if (ok) {
        c_text_items[value].font = fnt;
    }
}

void editor::on_textEdit_textChanged(QString newText)
{
    QString value = c_items.key(ui_scene->selectedItems().at(0));
    c_text_items[value].text = newText;
}

void editor::on_delButton_clicked()
{
    ui_scene->removeItem(ui_scene->selectedItems().at(0));
}

void editor::add_element_to_scene(QString type, float startX, float startY, float width, float heith, QBrush brush, QFont font, QString text, bool to_removal   ) {
    if (type.contains("line")) {
        ui_scene->removeItem(c_items[type]);
        //переводим прямоугольные координаты в полярные
        float len = sqrt(width*width + heith*heith);
        float angl;
        if (width) {
            angl = atan(heith / width) * 180 / M_PI;
        } else {
            angl = 90;
        }

        //c_items[type] = imagelinethick(ui_scene, 0, 0, len, 0, 5);
        c_items[type] = ui_scene->addRect(0, 0, len, text.toFloat());


        //qDebug() << "text IS " << text.toFloat();
        text = QString::number(angl);
        c_items[type]->rotate(angl);

        float new_angl = c_items[type]->rotation();
        qDebug() << "rotation from item is " << new_angl;


        to_removal = false;
    } else {
        c_items[type] = ui_scene->addRect(0, 0, width, heith, QPen(Qt::black), brush);

    }
    c_items[type]->setPos(startX, startY);
    c_text_items[type].font = font;
    c_text_items[type].text = text;
    c_items[type]->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

    if (to_removal) {
        ui_scene->removeItem(c_items[type]);
    }


}

void editor::on_addRectButton_clicked()
{
    static int i = 0;
    QString value = "text" + QString::number(i++);
    add_element_to_scene(value, 0, 0, 200, 50, QBrush(Qt::yellow), QFont("Arial", 20), "text", false);

}

void editor::on_loadButton_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open cennic template"), fileName, tr("Cennic templates (*.cen)"));
    if (fileName == "") {
        qDebug() << "Please select file name";
        return;
    }
    doc.clear();

    QDomElement domElement;
    this->on_clearButton_clicked();
    ui_scene->removeItem(baseRect);

    QFile file;
    file.setFileName(fileName);

    if (file.open(QIODevice::ReadOnly)) {
    if (doc.setContent(&file)) {
        domElement = doc.documentElement();

        load_xml_data_into_editor(&domElement);
    }
    } else {
        //error что файл не открывается
        qDebug() << "cant open file";
        return;
    }
}

void editor::load_xml_data_into_editor(QDomElement *domElement) {
    ui_scene->clear();
    c_items_initialize();

    QDomNode node = domElement->firstChild();
    while (!node.isNull()){
        if (node.isElement()) {
            QDomElement elemment = node.toElement();

            domElement =  &elemment;
            QString element = domElement->tagName();

            //устанавливаем переменные из XML-файла
            //некоторые из них могут и не быть - пофиг, есть дефолтные значения
            QString method = domElement->attribute("method", "");
            float width =  domElement->attribute("width", "10").toFloat();
            float heith =  domElement->attribute("heith", "10").toFloat();
            float startX = domElement->attribute("startX", "0").toFloat();
            float startY = domElement->attribute("startY", "0").toFloat();
            int font_size = domElement->attribute("font-size", "10").toInt();
            QString linethick = domElement->attribute("linethick", "3");
            QString font_family = domElement->attribute("font-family", "sans");
            QFont font(font_family, font_size);
            QString rubSymbol = domElement->attribute("rub", "RUR");
            QString kopSymbol = domElement->attribute("kop", "KOP");
            QString text = domElement->text();
            //text = codec_utf8->toUnicode(text.toAscii());

            if (element == "name") {
                ui_shalon_nameEdit->setText(text);
                qDebug() << "Shablon name is " << text;
            }

            if (element == "base" ) {
                baseRect = ui_scene->addRect(0, 0, width, heith);
                qDebug() << "width = " << width;
                set_spin_value(ui_widthSpin, width);
                set_spin_value(ui_heithSpin, heith);
            }

            if (element == "line" ) {
                add_element_to_scene("line"+QString::number(line_number++), startX, startY, width, heith, linethick.toFloat());

                //QGraphicsLineItem* line = ui_scene->addLine(startX, startY, startX + width, startY + heith, QPen(Qt::black));
                //line.setFlags();
                //line->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            }

            if (element == "barcode") {
                add_element_to_scene("barcode", startX, startY,
                                     width, heith, QBrush(Qt::darkBlue),
                                     font, "216975", 0);
            }

            if (element == "good") {
                add_element_to_scene("good", startX, startY,
                                     width, heith, QBrush(Qt::green),
                                     font, "Belizna electra", 0);
            }

            if (element == "text") {
                if (method == "textInBox") {
                    static int i = 0;
                    //QString text = domElement->text();
                    //qDebug() << "text in box is " << text;
                    add_element_to_scene("text"+QString::number(i++), startX, startY,
                                         width, heith, QBrush(Qt::yellow),
                                         font, text, 0);
                }

                if (method == "date") {
                    add_element_to_scene("date", startX, startY,
                                         width, heith, QBrush(Qt::magenta),
                                         font, domElement->text(), 0);
                }
                if (method == "nomer") {
                    add_element_to_scene("nomer", startX, startY,
                                         width, heith, QBrush(Qt::blue),
                                         font, "77216", 0);
                }
                if (method == "price") {

                    QString money = rubSymbol + "::" + kopSymbol;
                    add_element_to_scene("price", startX, startY,
                                         width, heith, QBrush(Qt::red),
                                         font, money, 0);
                }
                if (method == "priceOld") {
                    QString money = rubSymbol + "::" + kopSymbol;
                    add_element_to_scene("priceOld", startX, startY,
                                         width, heith, QBrush(Qt::lightGray),
                                         font, money, 0);                    }
                if (method == "barcodeAsText") {
                    add_element_to_scene("barcodeAsText", startX, startY,
                                         width, heith, QBrush(Qt::darkYellow),
                                         font, "4600702003840", 0);
                }
            }

        }
        node = node.nextSibling();
    }

    on_previewButton_clicked();

}

void editor::set_file_name(QString name) {
    this->fileName = name;
}

void editor::on_setLeftButton_clicked()
{
    QGraphicsItem* item = ui_scene->selectedItems().at(0);
    item->setX(0);
}


void editor::on_setTopButton_clicked()
{
    QGraphicsItem* item = ui_scene->selectedItems().at(0);
    item->setY(0);
}


void editor::on_setRightButton_clicked()
{
    QGraphicsItem* item = ui_scene->selectedItems().at(0);
    item->setX(ui_widthSpin->value() - item->sceneBoundingRect().width());
}

void editor::on_setButtomButton_clicked()
{
    QGraphicsItem* item = ui_scene->selectedItems().at(0);
    item->setY(ui_heithSpin->value() - item->sceneBoundingRect().height());
}

void editor::on_barTextButton_clicked()
{
    switch_cennic_element(c_items["barcodeAsText"]);
}

void editor::on_zoomOutButton_clicked()
{
    ui_view->scale(0.5, 0.5);
}

void editor::on_zoomInButton_clicked()
{
    ui_view->scale(2, 2);
}

void editor::on_lineButton_clicked()
{

    AddLineMaster* ln_master = new AddLineMaster;
    connect(ln_master, SIGNAL(values_changed(QString,float,float,float,float,float)), SLOT(add_element_to_scene(QString,float,float,float,float,float)));

    if (ui_scene->selectedItems().count() == 1) {
        QGraphicsItem* item = ui_scene->selectedItems().at(0);
        QString item_value = c_items.key(item);
        if (item_value.contains("line")) {
            //вычисляем параметры линии
            float startX = item->x();
            float startY = item->y();
            float rotation = c_text_items[item_value].text.toFloat();
            float width, heith;
            item->rotate(0-rotation);
            float linethick = item->sceneBoundingRect().height();
            float len = item->sceneBoundingRect().width();
            width = len * cos(rotation/180*M_PI);
            heith = len * sin(rotation/180*M_PI);
            item->rotate(rotation);
            ln_master->set_values(startX, startY, width, heith, linethick);
            //qDebug() << "item value = " << item_value;
            ln_master->set_line_number(item_value.split("line").at(1).toInt());
        } else {
            ln_master->set_line_number(line_number++);
        }


    } else {
        ln_master->set_line_number(line_number++);
    }

    ln_master->exec();

}
