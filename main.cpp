#include <QtGui/QApplication>
#include "cengen.h"

int main(int argc, char *argv[])
{
    bool get_name = false;
    QString org_name = "IP_Volkov";
    QString app_name = "CenGen";
    QFile *file = new QFile("org_name.conf");
    if (!file->exists()) get_name = true;
    file->open(QFile::ReadOnly);
    if (!file->isOpen()) get_name = true;
    //file->exists()
    if (get_name) {
        //открываем диалоговое окно, вводим название организации

        //сохраняем полученный результат в файл
        file->open(QFile::WriteOnly);
        QTextStream out(file);
        out << "org_name = " + org_name + "\n";
        out << "app_name = " + app_name + "\n";
        //file << org_name;

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
                    qDebug() << "NAME is " << name;
                }
                if (line.contains("app_name")) {
                    name = line;
                    if (name.split("=").count()>1) {
                        name = name.split("=").at(1).trimmed();
                        app_name = name;
                    }
                    qDebug() << "APP is " << name;
                }
            }
            qDebug() << line;
        }
    }
    file->close();

    //устанавливаем название организации
    QCoreApplication::setOrganizationName(org_name);
    QCoreApplication::setApplicationName(app_name);


    QApplication a(argc, argv);

    QTranslator myTranslator;
    myTranslator.load("cengen_ru.qm", ".");
    a.installTranslator(&myTranslator);

    cengen w;
    w.set_org_name(org_name, app_name);

    w.show();
    return a.exec();
}
