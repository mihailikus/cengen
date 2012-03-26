#ifndef BVALIDATOR_H
#define BVALIDATOR_H

#include <QValidator>

class BarcodeValidator : public QValidator {
public:
    BarcodeValidator(QObject* parent)
        : QValidator(parent)
    {

    }

    virtual State validate(QString& str, int& ) const
    {
        QString tmp;
        QRegExp rxp = QRegExp ("[0-9]");
        for (int i = 0; i<str.length(); i++) {
            tmp = str.at(i);
            if (!tmp.contains(rxp) && tmp != "*") {
                return Invalid;
            }
        }

        if (str.length()>13) {
            return Invalid;
        }
        return Acceptable;

    }
};


#endif // BVALIDATOR_H
