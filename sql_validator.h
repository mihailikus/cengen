#ifndef SQL_VALIDATOR_H
#define SQL_VALIDATOR_H
#include <QValidator>

class SqlValidator : public QValidator {
public:
    SqlValidator(QObject* parent)
        : QValidator(parent)
    {

    }

    virtual State validate(QString& str, int& ) const
    {
        if (str.contains("SELECT", Qt::CaseInsensitive)) {
            return Invalid;
        }
        return Acceptable;

    }
};
#endif // SQL_VALIDATOR_H
