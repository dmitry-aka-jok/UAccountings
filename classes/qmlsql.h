#ifndef QMLSQL_H
#define QMLSQL_H

#include <QObject>
#include <QQmlEngine>

#include "qmlsqltable.h"

#include "datapipe.h"

class QmlSql : public QObject
{
    Q_OBJECT

public:
    enum FieldType {
        PK,
        FK,
        String,
        Int,
        Bool
    };
    Q_ENUM(FieldType)


    explicit QmlSql(QObject *parent = nullptr);

    Q_INVOKABLE void defineTable(QString name, QVariantMap fields);
    Q_INVOKABLE QmlSqlTable *table(QString name);

    static QString formatToSQL(QVariant val);

private:
    Datapipe *datapipe;

};

//Q_DECLARE_METATYPE(QmlSql::FieldType)

#endif // QMLSQL_H
