#ifndef SQL_VALIDATOR_H
#define SQL_VALIDATOR_H
#include <QValidator>

class SqlValidator : public QValidator {
public:
    SqlValidator(QObject* parent)
        : QValidator(parent)
    {

    }

    virtual State validate(QString& str, int& pos) const
    {
        //pos++;
        QRegExp rxp = QRegExp ("SELECT");
        if (str.contains(rxp)) {
            str = "NAFIG-NAFIG";
            return Invalid;
        }
        return Acceptable;

    }
};
#endif // SQL_VALIDATOR_H
