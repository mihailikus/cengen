#ifndef TNVALIDATOR_H
#define TNVALIDATOR_H
class TNValidator : public QValidator {
public:
    TNValidator(QObject* parent)
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
        if (str.length()>6) {
            return Invalid;
        }
        return Acceptable;

    }
};
#endif // TNVALIDATOR_H
