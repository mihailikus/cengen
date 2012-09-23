#include "xmlTextEditor.h"


xmlTextEditor::xmlTextEditor(QString name) {
    fileName = name;
    codec_utf8 = QTextCodec::codecForName("UTF-8");


    //сохранить
    action_save = new QAction(QIcon(":/share/images/resources/save.png"),
                            tr("Save file"), this);
    action_save->setShortcut(QKeySequence("Ctrl+S"));
    action_save->setToolTip(tr("Save file ") + fileName);
    connect(action_save, SIGNAL(triggered()), SLOT(on_action_save()));

    action_update = new QAction(tr("Update file"), this);
    action_update->setShortcut(QKeySequence("F5"));
    connect(action_update, SIGNAL(triggered()), SLOT(on_action_update()));

    action_exit = new QAction(tr("Save and exit"), this);
    action_exit->setShortcut(QKeySequence("Esc"));
    connect(action_exit, SIGNAL(triggered()), SLOT(on_action_exit()));


    addAction(action_save);
    addAction(action_update);
    addAction(action_exit);


    //            QPalette pal = txt->palette();
    //            pal.setColor(QPalette::Base, Qt::darkBlue);
    //            pal.setColor(QPalette::Text, Qt::yellow);
    //            txt->setPalette(pal);


    QFont fnt("Lucida Console", 9, QFont::Normal);
    this->document()->setDefaultFont(fnt);


    resize(640, 480);

    this->loadFile(fileName);



}

xmlTextEditor::~xmlTextEditor() {
    saveFile(fileName);
}

void xmlTextEditor::loadFile(QString name) {
//    QDomDocument doc;

    QFile file;
    file.setFileName(name);

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray tm = file.readAll();
        QString atm;
        atm = tm;
        this->setText(atm);
        new SyntaxHighlighter(this->document());

    }

}

void xmlTextEditor::saveFile(QString name) {
    QFile file;
    file.setFileName(name);

    if (file.open(QIODevice::WriteOnly)) {

        QTextStream out(&file);
        out.setCodec(codec_utf8);
        out << this->toPlainText();

        file.close();
    }
}

void xmlTextEditor::on_action_save() {
    saveFile(fileName);
}

void xmlTextEditor::on_action_update() {
    loadFile(fileName);
}

void xmlTextEditor::on_action_exit() {
    saveFile(fileName);
    this->close();
}
