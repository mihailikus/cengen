#include <QtGui/QApplication>
#include "cengen.h"
#include "file_worker.h"

int main(int argc, char *argv[])
{
    // Под Windows устанавливаем кодировку cp1251
    #ifdef Q_WS_WIN
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    // Под остальными ОС - utf8
    #else
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    #endif

    QString param;

    QString org_file_name;
    org_file_name = "org_name.conf";
    for (int i = 0; i<argc; i++) {
        param = codec->toUnicode(argv[i]);
        if (check_extention(param, ".conf")) {
                    //ключ c - передается файл с названием организации
                    org_file_name = param;
        }
    }

    bool get_name = false;
    QString org_name = "IP_Volkov";
    QString app_name = "CenGen";
    QFile *file = new QFile(org_file_name);
    if (!file->exists()) get_name = true;
    file->open(QFile::ReadOnly);
    if (!file->isOpen()) get_name = true;
    if (get_name) {
        //открываем диалоговое окно, вводим название организации

        //сохраняем полученный результат в файл
        file->open(QFile::WriteOnly);
        QTextStream out(file);
        out << "org_name = " + org_name + "\n";
        out << "app_name = " + app_name + "\n";

    } else {
        //читаем название организации из файла
        QString name;
        QByteArray line;
        while (!file->atEnd()) {
            line = file->readLine();
            if (line.at(0) != '#') {
                if (line.contains("org_name")) {
                    name = line;
                    if (name.split("=").count()>1) {
                        name = name.split("=").at(1).trimmed();
                        org_name = name;
                    }
                }
                if (line.contains("app_name")) {
                    name = line;
                    if (name.split("=").count()>1) {
                        name = name.split("=").at(1).trimmed();
                        app_name = name;
                    }
                }
            }
        }
    }
    file->close();
    //qDebug() << "Org name " << org_name;

    //устанавливаем название организации
    QCoreApplication::setOrganizationName(org_name);
    QCoreApplication::setApplicationName(app_name);

    QFileInfo *fi = new QFileInfo;
    fi->setFile(org_file_name);

    QString path = fi->absolutePath();

    QApplication a(argc, argv);

    QTranslator myTranslator;
    myTranslator.load("cengen_ru.qm", path);
    a.installTranslator(&myTranslator);

    cengen w;
    w.set_org_name(org_name, app_name);

    for (int i = 0; i<argc; i++) {
        param = codec->toUnicode(argv[i]);

        if (check_extention(param, ".tov")) {
            w.open_tovar_list(param);
        }
        if (check_extention(param, ".fli")) {
            w.turn_filter_ON();
            w.load_filter_settings_file(param);
        }
        if (check_extention(param, ".das")) {
            w.load_source_settings_file(param);
        }
        if (check_extention(param, ".dbf")) {
            qDebug() << "Trying to load dbf file";
            w.save_source_settings_file("_$tmp.das");
            bool good = w.set_source_from_dbf(param);
            if (good) w.load_all_records();
            w.load_source_settings_file("_$tmp.das");
            w.repaint();
        }
        if (check_extention(param, "frf")) {
            w.set_ext_shablon_name(param);
        }
        if (check_extention(param, "csf")) {
            QLabel *lbl = new QLabel("Please wait! Executing scenarium file");
            lbl->show();
            w.execute_macro_file(param);
            lbl->hide();
        }
    }

    w.show();
    return a.exec();
}
