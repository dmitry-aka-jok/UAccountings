#ifndef QMLSQLTABLE_H
#define QMLSQLTABLE_H

#include <QObject>
#include <QVariant>

#include "qmlsqlmodel.h"

class QmlSqlTable : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString table MEMBER m_table READ table WRITE setTable NOTIFY tableChanged)

public:
    explicit QmlSqlTable(QObject *parent = nullptr);


    void setTable(const QString &table);
    void setFields(const QVariantMap &fields);

    QString table();
    QVariantMap fields();

    bool validate();

    Q_INVOKABLE QmlSqlModel* select(QVariantMap query);

    Q_INVOKABLE bool insert(QVariantMap fields, QVariantList returns);
    Q_INVOKABLE bool update(QVariantMap fields, QVariantMap conditions);
    Q_INVOKABLE bool update(QVariantMap fields, QString conditions);
    Q_INVOKABLE bool insert_or_update(QVariantMap fields, QVariantMap conditions);
    Q_INVOKABLE bool remove(QVariantMap fields);

    QString lastError();

signals:
    void tableChanged();
    void fieldsChanged();

private:
    QString m_table;
    QVariantMap m_fields;
    QString m_lastError;
};

#endif // QMLSQLTABLE_H
