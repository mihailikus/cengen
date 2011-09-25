#ifndef SQL_CONFIG_H
#define SQL_CONFIG_H

#include <QString>

class SqlConfig {
    //переменные для соединения с базой данных
public:
    QString dbHost;
    QString dbName;
    QString dbUser;
    QString dbPass;
    QString tbName;
};

#endif // SQL_CONFIG_H
