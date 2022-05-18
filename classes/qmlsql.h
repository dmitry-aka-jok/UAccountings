#ifndef QMLSQL_H
#define QMLSQL_H

#include <QObject>

class QmlSql : public QObject
{
    Q_OBJECT

public:
    enum FieldType {
        PK,
        FK,
        String,
        Int,
        Bool,
        Numeric
    };
    Q_ENUM(FieldType)



    static QmlSql* instance()
    {
        static QmlSql singleton;
        return &singleton;
    }

    static QString formatToSQL(QVariant val);
};

#endif // QMLSQL_H
