#include <QtGui/QApplication>
#include "cengen.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("IP_Volkov");
    QCoreApplication::setApplicationName("CenGen");
    QApplication a(argc, argv);
    cengen w;
    w.show();
    return a.exec();
}