#include <QtGui/QApplication>
#include "cengen.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("UG_Dvor_34");
    QCoreApplication::setApplicationName("CenGen");
    QApplication a(argc, argv);

    QTranslator myTranslator;
    myTranslator.load("cengen_ru.qm", ".");
    a.installTranslator(&myTranslator);


    cengen w;
    //w.translate();
    w.show();
    return a.exec();
}
