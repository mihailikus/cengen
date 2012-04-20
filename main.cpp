#include <QtGui/QApplication>
#include "cengen.h"

int main(int argc, char *argv[])
{
    QString org_file_name;
    org_file_name = "org_name.conf";
    for (int i = 0; i<argc; i++) {
        if (argv[i][0]=='-') {
            if (argv[i][1] == 'c') {
                    //ключ c - передается файл с названием организации
                    qDebug() << "tovar file " << i << argv[i+1];
                    org_file_name = argv[i+1];
            }
            i++;
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

    //устанавливаем название организации
    QCoreApplication::setOrganizationName(org_name);
    QCoreApplication::setApplicationName(app_name);

    QFileInfo *fi = new QFileInfo;
    fi->setFile(org_file_name);

    QString path = fi->absolutePath();
    qDebug() << path;


    //QString path = fi->absoluteDir();
    //path += "cengen_ru.qm"


    QApplication a(argc, argv);

    QTranslator myTranslator;
    myTranslator.load("cengen_ru.qm", path);
    a.installTranslator(&myTranslator);

    cengen w;
    w.set_org_name(org_name, app_name);

    for (int i = 0; i<argc; i++) {
        if (argv[i][0]=='-') {
            if (argv[i][1] == 'd') {
                //ключ d - передаются настройки источника данных
                    qDebug() << "source file " << i << argv[i+1];
                    QString sourceFile = argv[i+1];
                    qDebug() << sourceFile;
                    w.load_source_settings_file(sourceFile);
            }

            if (argv[i][1] == 'f') {
                //ключ f - передаются настройки фильтра
                    qDebug() << "filter file " << i << argv[i+1];
                    QString filterFile = argv[i+1];
                    qDebug() << filterFile;
                    w.turn_filter_ON();
                    w.load_filter_settings_file(filterFile);
            }

            if (argv[i][1] == 't') {
                    //ключ t - передается список товаров
                    qDebug() << "tovar file " << i << argv[i+1];
                    QString tovarFile = argv[i+1];
                    qDebug() << tovarFile;
                    w.open_tovar_list(tovarFile);
            }

            i++;
        }

    }


    w.show();
    return a.exec();
}
