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
        //pos++;
        QRegExp rxp = QRegExp ("\\D");
        if (str.contains(rxp)) {
            return Invalid;
        }
        if (str.length()>13) {
            return Invalid;
        }
        return Acceptable;

    }
};


#endif // BVALIDATOR_H
