#ifndef QMLSQLTABLE_H
#define QMLSQLTABLE_H

#include <QObject>
#include <QVariant>

#include "datapipe.h"

class QmlSqlTable : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString table MEMBER m_table READ table WRITE setTable NOTIFY tableChanged)
   // Q_PROPERTY(QString fields MEMBER m_fields READ fields WRITE setFields NOTIFY fieldsChanged)

public:
    explicit QmlSqlTable(Datapipe *datapipe, QObject *parent = nullptr);

    void setTable(QString table);
    void setFields(QVariantMap fields);

    QString table();
    QVariantMap fields();

    bool validate();

    void get(QVariantMap query);

    QString lastError();

signals:
    void tableChanged();
    void fieldsChanged();

private:
    QString m_table;
    QVariantMap m_fields;
    Datapipe* datapipe;
    QString m_lastError;
};

#endif // QMLSQLTABLE_H
