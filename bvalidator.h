#ifndef BVALIDATOR_H
#define BVALIDATOR_H

#include <QValidator>

class BarcodeValidator : public QValidator {
public:
    BarcodeValidator(QObject* parent)
        : QValidator(parent)
    {

    }

    virtual State validate(QString& str, int& pos) const
    {
        QString tmp;
        QRegExp rxp = QRegExp ("[0-9]");
        for (int i = 0; i<str.length(); i++) {
            tmp = str.at(i);
            if (!tmp.contains(rxp) && tmp != "*") {
                return Invalid;
            }
        }
        if (pos == 1 && str[0] == '0') {
            return Invalid;
        }
        if (str.length()>13) {
            return Invalid;
        }
        return Acceptable;

    }
};


#endif // BVALIDATOR_H
