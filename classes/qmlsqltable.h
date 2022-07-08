#ifndef QMLSQLTABLE_H
#define QMLSQLTABLE_H

#include <QObject>
#include <QVariant>

#include "qmlsqlmodel.h"
#include "qmlsqlquery.h"

#include "macro.h"

class QmlSqlTable : public QObject
{
    Q_OBJECT

    UA_PROP_RW(QString, table, setTable)

public:
    explicit QmlSqlTable(QObject *parent = nullptr);

    // compare and update to last level
    bool validate();

    Q_INVOKABLE QmlSqlQuery* select(QVariantMap query);
    Q_INVOKABLE QmlSqlModel* model(QVariantMap query);

    Q_INVOKABLE bool insert(QVariantMap fields, QVariantList returns);
    Q_INVOKABLE bool update(QVariantMap fields, QVariantMap conditions);
    Q_INVOKABLE bool update(QVariantMap fields, QString conditions);
    Q_INVOKABLE bool insert_or_update(QVariantMap fields, QVariantMap conditions);
    Q_INVOKABLE bool remove(QVariantMap fields);

    QString lastError();

signals:

private:
    QString m_lastError;
};

#endif // QMLSQLTABLE_H
