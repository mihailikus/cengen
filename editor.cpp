#include "editor.h"

editor::editor(QWidget *parent, Qt::WFlags f) :
    QDialog(parent, f)
{
    scene = new QGraphicsScene;
    view = new QGraphicsView(scene, this);
    connect(scene, SIGNAL(selectionChanged()), SLOT(on_scene_selected()));
    connect(scene, SIGNAL(changed(QList<QRectF>)), SLOT(generate_preview()));

    pre_scene = new QGraphicsScene;
    pre_view = new QGraphicsView(pre_scene, this);
    pre_view->scale(0.5, 0.5);


    layout = new QGridLayout;

    isSaved = false;
    isLoaded = false;
    codec_utf8 = QTextCodec::codecForName("UTF-8");
    fileName = "template.cen";


    fileNameChanged = true;
    fileName = "new_template.cen";
    statusFileNameLabel = new QLabel(fileName);

    makeActions();
    makeToolBars();

    brIs = false;
    txIs = false;
     gIs = false;
     iIs = false;
     lIs = false;

    onXchanged = false;
    onYchanged = false;

    QLabel *lb = new QLabel(" ");
    layout->addWidget(secToolBar, 0, 0, 1, 2);
    layout->addWidget(mainToolBar, 1, 0, 1, 2);
    layout->addWidget(lb, 2, 1);
    layout->addWidget(view, 3, 0);
    layout->addWidget(pre_view, 3, 1);
    this->setLayout(layout);


}

editor::~editor()
{
    if (brIs) {
        brToolBar->hide();
        brIs = false;
    }
    if (txIs) {
        txToolBar->hide();
        txIs = false;
    }
    if (gIs) {
        gToolBar->hide();
        gIs = false;
    }
    if (iIs) {
        iToolBar->hide();
        iIs = false;
    }
    if (lIs) {
        lToolBar->hide();
        lIs = false;
    }
}

bool editor::set_file_name(QString name) {
    //qDebug() << "Setting filename " << name;
    if (isLoaded) {
        fileName= name;
        return true;
    }

    ///устанавливает файл шаблона
    QFileInfo fi(name);
    qDebug() << "New file name: " << name;
    if (name == fileName) {
        fileNameChanged = false;
        return true;
    }

    if (fi.exists()) {
        fileName = name;
        fileNameChanged = true;
        statusFileNameLabel->setText(fileName);

        doc.clear();

        QDomElement domElement;
        //this->on_clearButton_clicked();
        //ui_scene->removeItem(baseRect);

        QFile file;
        file.setFileName(fileName);

        if (file.open(QIODevice::ReadOnly)) {
            if (doc.setContent(&file)) {
                domElement = doc.documentElement();

                load_xml_data_into_editor(&domElement);
            } else {
                qDebug() << "file wrong";
                return false;
            }
        } else {
            //error что файл не открывается
            qDebug() << "cant open file";
            return false;
        }

        return true;
    }
    return false;
}

void editor::makeActions() {
    action_new = new QAction(tr("New"), this);
    action_new->setShortcut(QKeySequence("Ctrl+N"));
    connect(action_new, SIGNAL(triggered()), SLOT(on_action_new()));

    action_open = new QAction(tr("Open"), this);
    action_open->setShortcut(QKeySequence("Ctrl+O"));
    connect(action_open, SIGNAL(triggered()), SLOT(on_action_open()));

    action_save = new QAction(tr("Save"), this);
    action_save->setShortcut(QKeySequence("Ctrl+S"));
    connect(action_save, SIGNAL(triggered()), SLOT(on_action_save()));

    action_del = new QAction(tr("Del"), this);
    connect(action_del, SIGNAL(triggered()), SLOT(on_action_del()));
    action_del->setEnabled(false);

    action_add_barcode = new QAction(tr("Barcode"), this);
    connect(action_add_barcode, SIGNAL(triggered()),
            SLOT(on_action_add_barcode()));

    action_add_barcode = new QAction(tr("Barcode"), this);
    connect(action_add_barcode, SIGNAL(triggered()),
            SLOT(on_action_add_barcode()));

    action_add_barcodeText = new QAction(tr("brText"), this);
    connect(action_add_barcodeText, SIGNAL(triggered()),
            SLOT(on_action_add_barcodeText()));

    action_add_nomer = new QAction(tr("Nomer"), this);
    connect(action_add_nomer, SIGNAL(triggered()),
            SLOT(on_action_add_nomer()));

    action_add_good = new QAction(tr("Good"), this);
    connect(action_add_good, SIGNAL(triggered()),
            SLOT(on_action_add_good()));

    action_add_date = new QAction(tr("Date"), this);
    connect(action_add_date, SIGNAL(triggered()),
            SLOT(on_action_add_date()));

    action_add_price = new QAction(tr("Price"), this);
    connect(action_add_price, SIGNAL(triggered()),
            SLOT(on_action_add_price()));

    action_add_priceOld = new QAction(tr("PrOld"), this);
    connect(action_add_priceOld, SIGNAL(triggered()),
            SLOT(on_action_add_priceOld()));

    action_add_image = new QAction(tr("Image"), this);
    connect(action_add_image, SIGNAL(triggered()),
            SLOT(on_action_add_image()));

    action_add_line = new QAction(tr("Line"), this);
    connect(action_add_line, SIGNAL(triggered()),
            SLOT(on_action_add_line()));

    action_add_textInBox = new QAction(tr("Text"), this);
    connect(action_add_textInBox, SIGNAL(triggered()),
            SLOT(on_action_add_textInBox()));

    action_zoom_in = new QAction(tr("+", "Zoom in"), this);
    connect(action_zoom_in, SIGNAL(triggered()), SLOT(on_zoom_in()));

    action_zoom_out = new QAction(tr("-", "Zoom out"), this);
    connect(action_zoom_out, SIGNAL(triggered()), SLOT(on_zoom_out()));

    buttonRight = new QAction(tr("<", "Move right"), this);
    buttonLeft = new QAction(tr(">", "Move left"), this);
    buttonTop = new QAction(tr("/\\", "Move top"), this);
    buttonButtom = new QAction(tr("\\/", "Move buttom"), this);
    buttonCenter = new QAction(tr("(.)"), this);
    connect(buttonRight, SIGNAL(triggered()), SLOT(on_button_right()));
    connect(buttonLeft, SIGNAL(triggered()), SLOT(on_button_left()));
    connect(buttonTop, SIGNAL(triggered()), SLOT(on_button_top()));
    connect(buttonButtom, SIGNAL(triggered()), SLOT(on_button_buttom()));
    connect(buttonCenter, SIGNAL(triggered()), SLOT(on_button_center()));
    buttonRight->setEnabled(false);
    buttonLeft->setEnabled(false);
    buttonTop->setEnabled(false);
    buttonButtom->setEnabled(false);
    buttonCenter->setEnabled(false);


}

void editor::makeToolBars() {
    mainToolBar = new QToolBar();
    mainToolBar->addAction(action_new);
    mainToolBar->addSeparator();
    mainToolBar->addAction(action_save);
    mainToolBar->addAction(action_open);
    mainToolBar->addSeparator();
    mainToolBar->addAction(action_del);
    mainToolBar->addSeparator();
    mainToolBar->addAction(action_add_barcode);
    mainToolBar->addAction(action_add_barcode);
    mainToolBar->addAction(action_add_barcodeText);
    mainToolBar->addAction(action_add_nomer);
    mainToolBar->addAction(action_add_good);
    mainToolBar->addAction(action_add_date);
    mainToolBar->addAction(action_add_price);
    mainToolBar->addAction(action_add_priceOld);
    mainToolBar->addAction(action_add_image);
    mainToolBar->addAction(action_add_line);
    mainToolBar->addAction(action_add_textInBox);

    secToolBar = new QToolBar();
    lineWidth = new QSpinBox();
    lineHeith = new QSpinBox();
    lineWidth->setMaximum(3000);
    lineHeith->setMaximum(3000);
    connect(lineHeith, SIGNAL(valueChanged(int)), SLOT(on_baseSize_changed()));
    connect(lineWidth, SIGNAL(valueChanged(int)), SLOT(on_baseSize_changed()));

    secToolBar->addWidget(lineWidth);
    secToolBar->addWidget(lineHeith);

    secToolBar->addAction(action_zoom_in);
    secToolBar->addAction(action_zoom_out);

    secToolBar->addSeparator();

    //secToolBar->addWidget(buttonRight);
    secToolBar->addAction(buttonRight);
    secToolBar->addAction(buttonLeft);
    secToolBar->addAction(buttonTop);
    secToolBar->addAction(buttonButtom);
    secToolBar->addAction(buttonCenter);

    secToolBar->addSeparator();


    labelName = new QLineEdit(tr("Sample shablon"));
    secToolBar->addWidget(labelName);

}

void editor::load_xml_data_into_editor(QDomElement *domElement) {
///загружаем XML-шаблон в редактор
    qDebug() << "Loading xml shablon";

    isLoaded = true;

    scene->blockSignals(true);
    //scene->clear();

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
            bool font_bold = domElement->attribute("font-bold", "0").toInt();
            bool font_italic = domElement->attribute("font-italic", "0").toInt();

            QString linethick = domElement->attribute("linethick", "3");
            QString font_family = domElement->attribute("font-family", "sans");
            int br_lineAddition = domElement->attribute("addition", "25").toInt();
            int br_otstup = domElement->attribute("otstup", "10").toInt();
            QFont font(font_family, font_size);
            font.setBold(font_bold);
            font.setItalic(font_italic);
            QString rubSymbol = domElement->attribute("rub", "RUR");
            QString kopSymbol = domElement->attribute("kop", "KOP");
            QString text = domElement->text();

            if (element == "base" ) {
                new_shablon(width, heith);
                //baseRect = scene->addRect(0, 0, width, heith);
                //qDebug() << "width = " << width;
                //set_spin_value(ui_widthSpin, width);
                //set_spin_value(ui_heithSpin, heith);
            }
            if (element == "name") {
                labelName->setText(text);
            }
            if (element == "barcode") {
                qDebug() << "Adding barcode box";
                add_barcode(startX, startY, width, heith,
                            font_size, font_family, font_bold, font_italic,
                            br_lineAddition, br_otstup, tr("216975", "Example barcode text"));
            }
            if (element == "good") {
                add_good(startX, startY, width, heith,
                         font_size, font_family, font_bold, font_italic,
                         tr("Example good name Belizna Electra 1100 ml, this text is not impotant"));
            }
            if (element == "image") {
                add_image(startX, startY, width, heith, text);
            }
            if (element == "line") {
                add_line(startX, startY, width, heith, linethick.toFloat());
            }
            if (element == "text") {
                if (method == "textInBox") {
                    add_text(method, startX, startY, width, heith,
                             font_size, font_family, font_bold, font_italic,
                             text);
                }
                if (method == "barcodeAsText") {
                    text = tr("216975", "Example barcode text");
                    add_text(method, startX, startY, width, heith,
                             font_size, font_family, font_bold, font_italic,
                             text);
                }
                if (method == "date") {
                    add_text(method, startX, startY, width, heith,
                             font_size, font_family, font_bold, font_italic,
                             text);
                }
                if (method == "nomer") {
                    text = tr("21697", "Example tnomer");
                    add_text(method, startX, startY, width, heith,
                             font_size, font_family, font_bold, font_italic,
                             text);
                }
                if (method == "quantity") {
                    text = tr("12", "Example quantity");
                    add_text(method, startX, startY, width, heith,
                             font_size, font_family, font_bold, font_italic,
                             text);
                }
                if (method == "price") {

                    QString money = rubSymbol + "::" + kopSymbol;
                    add_text(method, startX, startY, width, heith,
                             font_size, font_family, font_bold, font_italic,
                             money);
                }
                if (method == "priceOld") {

                    QString money = rubSymbol + "::" + kopSymbol;
                    add_text(method, startX, startY, width, heith,
                             font_size, font_family, font_bold, font_italic,
                             money);
                }

                }
        }
        node = node.nextSibling();
    }
    scene->blockSignals(false);
//    generate_preview();

}

void editor::on_action_new() {
    new_shablon(lineWidth->value(), lineHeith->value());
    labelName->setText(tr("Sample shablon"));
    fileName = "template.cen";
    isSaved = false;
}

void editor::on_action_save() {
    //создаем превью (вместе с XML-документом)
    this->generate_preview();

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

            isSaved = true;
        }
        //qDebug() << "template:: \n " << doc.toString();

    }
}

void editor::on_action_open() {
    QString str = QFileDialog::getOpenFileName(this, tr("Open cennic template"), fileName, tr("Cennic templates (*.cen)"));
    if (str == "") {
        qDebug() << "Please select file name";
        return;
    }
    isLoaded = false;
    this->set_file_name(str);
}

void editor::on_action_del() {
    QList<QGraphicsItem *> lst = scene->selectedItems();
    for (int i = 0; i<lst.count(); i++) {
        scene->removeItem(lst.at(i));
        delete lst.at(i);
    }
    view->repaint();
}

void editor::new_shablon(int w, int h) {
    scene->clear();
    view->repaint();
    baseRect = scene->addRect(0, 0, w, h);
    lineWidth->setValue(w);
    lineHeith->setValue(h);
}

void editor::on_baseSize_changed() {
    scene->removeItem(baseRect);
    baseRect = scene->addRect(0, 0, lineWidth->value(), lineHeith->value());
}

void editor::on_zoom_in() {
    view->scale(2, 2);
    pre_view->scale(2, 2);
}

void editor::on_zoom_out() {
    view->scale(0.5, 0.5);
    pre_view->scale(0.5, 0.5);
}

void editor::on_action_add_barcode() {
    add_barcode(0, 0, 300, 65, 10, "Arial", false, false, 10, 5, tr("216975"));
}

void editor:: on_action_add_barcodeText(){
    add_text("barcodeAsText", 0, 0, 100, 40, 10, "Arial", false, false, tr("216975"));
}

void editor:: on_action_add_nomer(){
    add_text("nomer", 0, 0, 100, 40, 10, "Arial", false, false, tr("216975"));
}

void editor:: on_action_add_good(){
    add_good(0, 0, 300, 200, 30, "Arial", false, false, tr("Example good name Belizna Electra 1100 ml, this text is not impotant"));
}

void editor:: on_action_add_date(){
    add_text("date", 0, 0, 100, 40, 10, "Arial", false, false, tr("dd.MM.yy"));
}

void editor:: on_action_add_price(){
    add_text("price", 0, 0, 100, 40, 10, "Arial", false, false, tr("rub::kop"));
}

void editor:: on_action_add_priceOld(){
    add_text("priceOld", 0, 0, 100, 40, 10, "Arial", false, false, tr("rub::kop"));
}

void editor:: on_action_add_image(){
    QString str = QFileDialog::getOpenFileName(0, tr("Select image file"), ".", tr("images (*.jpg *.gif)"));

    if (str != "") {
        QPixmap *pxItem = new QPixmap(str);
        float w = pxItem->width();
        float h = pxItem->height();

        add_image(0, 0, w, h, str);

    }
}

void editor:: on_action_add_line(){
    add_line(0, 0, 200, 0, 10);
}

void editor:: on_action_add_textInBox(){
    add_text("textInBox", 0, 0, 100, 40, 10, "Arial", false, false, tr("Sample text"));
}


void editor::add_barcode(float startX, float startY, float w, float h,
                         int font_size, QString font_family, bool Bold, bool Italic,
                         int brAddition, int brOtstup,
                         QString text) {
    BarcodeBox *brBox = new BarcodeBox(w, h);
    brBox->setPos(startX, startY);
    brBox->setBrAddition(brAddition);
    brBox->setBrOtstup(brOtstup);
    brBox->setFont(font_size, font_family, Bold, Italic);
    brBox->setText(text);

    scene->addItem(brBox);
    connect(brBox, SIGNAL(some_changed()), SLOT(generate_preview()));
}

void editor::add_text(QString method, float startX, float startY, float w, float h, int font_size, QString font_family, bool Bold, bool Italic, QString text) {
    TextBox *txBox = new TextBox(w, h, method);

    txBox->setPos(startX, startY);
    txBox->setFont(font_size, font_family, Bold, Italic);
    txBox->setText(text);
    scene->addItem(txBox);
    connect(txBox, SIGNAL(some_changed()), SLOT(generate_preview()));
}

void editor::add_good(float startX, float startY, float w, float h, int font_size, QString font_family, bool Bold, bool Italic, QString text) {
    Good *gBox = new Good(w, h);
    gBox->setPos(startX, startY);
    gBox->setFont(font_size, font_family, Bold, Italic);
    gBox->setText(text);
    scene->addItem(gBox);
    connect(gBox, SIGNAL(some_changed()), SLOT(generate_preview()));
}

void editor::add_image(float startX, float startY, float w, float h, QString fileName) {
    Image *iBox = new Image(w, h, fileName);
    iBox->setPos(startX, startY);
    scene->addItem(iBox);
    connect(iBox, SIGNAL(some_changed()), SLOT(generate_preview()));
}

void editor::add_line(float startX, float startY, float w, float h, float lineThick) {
    //переводим прямоугольные координаты в полярные
    qDebug() << "w, h" << w << h;
    float len = sqrt(w*w + h*h);
    float angl;
    if (w) {
        angl = atan(h / w) * 180 / M_PI;
    } else {
        angl = 90;
    }

    qDebug() << "len, algl" << len << angl;


    Line *lBox = new Line(len, angl, lineThick);
    lBox->setPos(startX, startY);
    scene->addItem(lBox);
    connect(lBox, SIGNAL(some_changed()), SLOT(generate_preview()));
}

void editor::on_scene_selected() {
    if (brIs) {
        brToolBar->hide();
        brIs = false;
    }
    if (txIs) {
        txToolBar->hide();
        txIs = false;
    }
    if (gIs) {
        gToolBar->hide();
        gIs = false;
    }
    if (iIs) {
        iToolBar->hide();
        iIs = false;
    }
    if (lIs) {
        lToolBar->hide();
        lIs = false;
    }

    QList<QGraphicsItem*> lst = scene->selectedItems();
    if (!lst.count()) {
        //ничего не выделено, выходим
        action_del->setEnabled(false);
        buttonRight->setEnabled(false);
        buttonLeft->setEnabled(false);
        buttonTop->setEnabled(false);
        buttonButtom->setEnabled(false);
        buttonCenter->setEnabled(false);
        onXchanged = false;
        onYchanged = false;
    } else {
        buttonRight->setEnabled(true);
        buttonLeft->setEnabled(true);
        buttonTop->setEnabled(true);
        buttonButtom->setEnabled(true);
        action_del->setEnabled(true);
        buttonCenter->setEnabled(true);
    }
    if (lst.count() == 1) {
        //выделен один элемент - выведем его свойства
        //qDebug() << "Selected type: " << lst.at(0)->type();
        int tp = lst.at(0)->type();
        if (tp == 700) {
            //штрих-код
            BarcodeBox *brBox = (BarcodeBox*)lst.at(0);
            brToolBar = brBox->toolBar();
            layout->addWidget(brToolBar, 2, 0);
            brIs = true;
        }
        if (tp == 701) {
            //text in box
            TextBox *txBox = (TextBox*)lst.at(0);
            txToolBar = txBox->toolBar();
            //txToolBar->show();
            layout->addWidget(txToolBar, 2, 0);
            txIs = true;
        }
        if (tp == 702) {
            //good name field
            Good *gBox = (Good*)lst.at(0);
            gToolBar = gBox->toolBar();
            layout->addWidget(gToolBar, 2, 0);
            gIs = true;
        }
        if (tp == 703) {
            //image field
            Image *iBox = (Image*)lst.at(0);
            iToolBar = iBox->toolBar();
            layout->addWidget(iToolBar, 2, 0);
            iIs = true;
        }
        if (tp == 704) {
            //line field
            Line *lBox = (Line*)lst.at(0);
            lToolBar = lBox->toolBar();
            layout->addWidget(lToolBar, 2, 0);
            lIs = true;
        }

    }
}

void editor::on_button_right() {
    onXchanged = true;
    onYchanged = false;
    QList<QGraphicsItem*> lst = scene->selectedItems();
    if (lst.count() == 1) {
        //только один элемент - переместим его к правому краю
        lst.at(0)->setPos(0, lst.at(0)->y());

    } else {
        //выделено несколько элементов - выровняем по правому элементу
        float maxPos = baseRect->sceneBoundingRect().width();
        for (int i = 0; i<lst.count(); i++) {
            if (maxPos > lst.at(i)->x()) maxPos = lst.at(i)->x();
        }
        for (int i = 0; i<lst.count(); i++) {
            lst.at(i)->setPos(maxPos, lst.at(i)->y());
        }
    }
}

void editor::on_button_left() {
    onXchanged = true;
    onYchanged = false;
    QList<QGraphicsItem*> lst = scene->selectedItems();
    float len = baseRect->sceneBoundingRect().width();
    if (lst.count() == 1) {
        //только один элемент - переместим его к правому краю
        lst.at(0)->setPos(len-lst.at(0)->sceneBoundingRect().width(), lst.at(0)->y());

    } else {
        //выделено несколько элементов - выровняем по правому элементу
        float maxPos = 0;
        for (int i = 0; i<lst.count(); i++) {
            float curPos = lst.at(i)->x() + lst.at(i)->sceneBoundingRect().width();
            if (maxPos < curPos) maxPos = curPos;
        }
        for (int i = 0; i<lst.count(); i++) {
            lst.at(i)->setPos(maxPos - lst.at(i)->sceneBoundingRect().width(), lst.at(i)->y());
        }
    }
}

void editor::on_button_top() {
    onXchanged = false;
    onYchanged = true;
    QList<QGraphicsItem*> lst = scene->selectedItems();
    if (lst.count() == 1) {
        //только один элемент - переместим его к правому краю
        lst.at(0)->setPos(lst.at(0)->x(), 0);

    } else {
        //выделено несколько элементов - выровняем по правому элементу
        float maxPos = baseRect->sceneBoundingRect().height();
        for (int i = 0; i<lst.count(); i++) {
            if (maxPos > lst.at(i)->y()) maxPos = lst.at(i)->y();
        }
        for (int i = 0; i<lst.count(); i++) {
            lst.at(i)->setPos(lst.at(i)->x(), maxPos);
        }
    }
}

void editor::on_button_buttom() {
    onXchanged = false;
    onYchanged = true;
    QList<QGraphicsItem*> lst = scene->selectedItems();
    float len = baseRect->sceneBoundingRect().height();
    if (lst.count() == 1) {
        //только один элемент - переместим его к правому краю
        lst.at(0)->setPos(lst.at(0)->x(), len-lst.at(0)->sceneBoundingRect().height());

    } else {
        //выделено несколько элементов - выровняем по правому элементу
        float maxPos = 0;
        for (int i = 0; i<lst.count(); i++) {
            float curPos = lst.at(i)->y() + lst.at(i)->sceneBoundingRect().height();
            if (maxPos < curPos) maxPos = curPos;
        }
        for (int i = 0; i<lst.count(); i++) {
            lst.at(i)->setPos(lst.at(i)->x(), maxPos - lst.at(i)->sceneBoundingRect().height());
        }
    }
}

void editor::on_button_center() {
    QList<QGraphicsItem*> lst = scene->selectedItems();
    if (lst.count() == 1) {
        //выбран только один элемент - отцентруем относительно всего шаблона
        if (onXchanged) {
            float len = baseRect->sceneBoundingRect().width() / 2.0;
            lst.at(0)->setPos(len - lst.at(0)->sceneBoundingRect().width()/2, lst.at(0)->y());
        }
        if (onYchanged) {
            float len = baseRect->sceneBoundingRect().height() / 2.0;
            lst.at(0)->setPos(lst.at(0)->x(), len - lst.at(0)->sceneBoundingRect().height()/2);

        }

    } else {
        //выбрано несколько элементов - найдем общую середину и отцентруем
        if (onXchanged) {
            //float len = baseRect->sceneBoundingRect().width() / 2.0;
            float maxLen = 0;
            float centr = baseRect->sceneBoundingRect().width() / 2.0;
            for (int i = 0; i<lst.count(); i++) {
                if (lst.at(i)->sceneBoundingRect().width() > maxLen) {
                    maxLen = lst.at(i)->sceneBoundingRect().width();
                    centr = maxLen / 2.0 - lst.at(i)->x();
                }
            }

//            float centr = len - maxLen / 2.0;
            qDebug() << "center: " << centr;
            for (int i = 0; i<lst.count(); i++) {
                lst.at(i)->setPos(centr - lst.at(i)->sceneBoundingRect().width()/ 2, lst.at(i)->y());
            }

        }
    }
}

void editor::create_font_xml_element(QDomElement elem, AbstractItem *itm) {
    QString font_family = itm->fontFamily();
    elem.setAttribute("font-family", font_family);
    int font_size = itm->fontSize();
    elem.setAttribute("font-size", QString::number(font_size));
    elem.setAttribute("font-bold", itm->fontBold());
    elem.setAttribute("font-italic", itm->fontItalic());
}

void editor::generate_preview() {
//    isSaved = false;

    doc.clear();
    QDomNode xmlNode = doc.createProcessingInstruction("xml",
                                    "version=\"1.0\" encoding=\"UTF-8\"");
    doc.insertBefore(xmlNode, doc.firstChild());

    QDomElement cennic = doc.createElement("cennic");

    //устанавливаем название шаблона
    QDomElement elementName = doc.createElement("name");
    //QDomText domText = doc.createTextNode(ui_shalon_nameEdit->text());
    QDomText domText = doc.createTextNode(labelName->text());
    elementName.appendChild(domText);
    cennic.appendChild(elementName);

    //устанавливаем базовый размер - ширину и высоту
    QDomElement domElement = doc.createElement("base");

    domElement.setAttribute("width", QString::number(baseRect->sceneBoundingRect().width()));
    domElement.setAttribute("heith", QString::number(baseRect->sceneBoundingRect().height()));
    cennic.appendChild(domElement);

    Tovar tovar;

    QList<QGraphicsItem *> lst = scene->items();
    for (int i = 0; i<lst.count(); i++) {
        QDomElement elem;
        int tp = lst.at(i)->type();

        //AbstractItem *itm = (AbstractItem*)lst.at(i);
        if (tp == 700) {
            //штрих-код
            BarcodeBox *brBox = (BarcodeBox*)lst.at(i);

            elem = doc.createElement("barcode");
            elem.setAttribute("addition", brBox->addition());
            elem.setAttribute("otstup", brBox->otstup());
            create_font_xml_element(elem, brBox);

            tovar.barcode = brBox->text();
        }
        if (tp == 701) {
            //text field
            TextBox *txBox = (TextBox*)lst.at(i);
            elem = doc.createElement("text");

            elem.setAttribute("method", txBox->getMethod());
            create_font_xml_element(elem, txBox);
            QDomText dm = doc.createTextNode(txBox->text());
            elem.appendChild(dm);

            if (txBox->getMethod() == "nomer") {
                tovar.nomer_of_tovar = txBox->text().toInt();
            }
            if (txBox->getMethod() == "price" || txBox->getMethod() == "priceOld") {
                QStringList rub_kop = txBox->text().split("::");
                if (rub_kop.count()<2) {
                    rub_kop << tr("kop");
                }
                elem.setAttribute("rub", rub_kop.at(0));
                elem.setAttribute("kop", rub_kop.at(1));
            }
            if (txBox->getMethod() == "quantity") {
                tovar.quantity = txBox->text().toInt();
            }
            if (txBox->getMethod() == "barcodeAsText") {
                tovar.barcode = txBox->text();
            }
        }
        if (tp == 702) {
            //Good name field
            Good *gBox = (Good*)lst.at(i);
            elem = doc.createElement("good");
            create_font_xml_element(elem, gBox);
            tovar.name_of_tovar = gBox->text();
        }
        if (tp == 703) {
            //Image field
            Image *iBox = (Image*)lst.at(i);
            elem = doc.createElement("image");
            QDomText dm = doc.createTextNode(iBox->text());
            elem.appendChild(dm);
        }
        if (tp == 704) {
            //Line field
            //Line *lBox = (Line*)lst.at(i);
            elem = doc.createElement("line");
        }




        QString startX, startY, width, heith;
        startX = QString::number(lst.at(i)->mapToScene(0, 0).x());
        startY = QString::number(lst.at(i)->mapToScene(0, 0).y());

        if (tp != 704) {
            //если не линия - то размеры соответствуют размеру прямоугольника
            width = QString::number(lst.at(i)->sceneBoundingRect().width());
            heith = QString::number(lst.at(i)->sceneBoundingRect().height());
        } else {
            //если линия - то вычисляем размеры линии
            //вычисляем параметры линии
            Line *lBox = (Line*)lst.at(i);
            float rotation = lBox->angl();
            //qDebug () << "rotation is " << rotation;
            lBox->rotate(0-rotation);
            float linethick = lBox->sceneBoundingRect().height();
            float len = lBox->sceneBoundingRect().width();
            width = QString::number(len * cos(rotation/180*M_PI));
            heith = QString::number(len * sin(rotation/180*M_PI));
            lBox->rotate(rotation);
            elem.setAttribute("linethick", linethick);
        }




        elem.setAttribute("startX", startX);
        elem.setAttribute("startY", startY);
        elem.setAttribute("width", width);
        elem.setAttribute("heith", heith);

        cennic.appendChild(elem);
    }
    doc.appendChild(cennic);

    //qDebug() << doc.toString();

    pre_scene->clear();

    tovar.price1 = 299.90;
    tovar.price2 = 999.90;
    Cennic* preview_cennic = new Cennic(&tovar, doc.documentElement());
    QGraphicsItemGroup *pos = preview_cennic->render(pre_scene, 0, 0);

    pre_view->fitInView(0, 0, pos->x(), pos->y(), Qt::KeepAspectRatio);
    //qDebug() << "Preview finished";
}

QString editor::get_new_fileName() {
//    if (isSaved) {
        return fileName;
//    }
//    return "";
}

QDomDocument editor::get_new_shablon() {
    return doc;
}
