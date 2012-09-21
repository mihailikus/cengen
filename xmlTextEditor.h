#ifndef XMLTEXTEDITOR_H
#define XMLTEXTEDITOR_H

#include <QTextEdit>
#include <QDomDocument>
#include <QAction>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include "SyntaxHighlighter.h"

class QTextEdit;

class xmlTextEditor : public QTextEdit {
    Q_OBJECT

public:

    xmlTextEditor(QString fileName);
    ~xmlTextEditor();

private:
    QAction *action_save;
    QAction *action_update;
    QAction *action_exit;

    QString fileName;
    QTextCodec *codec_utf8;

private slots:
    void on_action_save();
    void on_action_update();
    void on_action_exit();
    void loadFile(QString name);
    void saveFile(QString name);

};


#endif // XMLTEXTEDITOR_H
