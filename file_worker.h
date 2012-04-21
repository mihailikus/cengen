#ifndef FILE_WORKER_H
#define FILE_WORKER_H

#include <QString>


bool check_extention(QString fileName, QString ext) {
    int ind = fileName.lastIndexOf(ext, -1, Qt::CaseInsensitive);
    if (ind + ext.length() != fileName.length() || ind == -1) {
        return false;
    }
    return true;
}

#endif // FILE_WORKER_H
