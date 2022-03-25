#ifndef QMLSQL_H
#define QMLSQL_H

#include <QObject>


#include "qmlsqltable.h"

#include "classes/datapipe.h"

class QmlSql : public QObject
{
    Q_OBJECT
public:
    explicit QmlSql(Datapipe *datapipe, QObject *parent = nullptr);

    Q_INVOKABLE void defineTable(QString name, QVariantMap fields);
    Q_INVOKABLE QmlSqlTable *table(QString name);

    static QString formatToSQL(QVariant val);
signals:

private:
    Datapipe* datapipe;
};

#endif // QMLSQL_H
