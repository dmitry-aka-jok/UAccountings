#ifndef QMLSQLQUERY_H
#define QMLSQLQUERY_H

#include <QSqlQuery>
#include <QObject>

#include "qmltypes.h"

struct PrepearedQuery
{
    QString query;
    QList<SqlType::Value> types;
    QList<int> digits;
};


class QmlSqlQuery : public QObject
{
    Q_OBJECT

public:
    explicit QmlSqlQuery(QObject *parent = nullptr);
    ~QmlSqlQuery();

    static PrepearedQuery queryString(const QString &table, QVariantMap queryMap);

    Q_INVOKABLE bool exec(const QString &table, QVariantMap queryMap);
    Q_INVOKABLE bool next();
    Q_INVOKABLE QVariant value(int index, QVariant deflt = QVariant());
    Q_INVOKABLE QVariant value(const QString &name, QVariant deflt = QVariant());
    Q_INVOKABLE bool isValid() const;

private:
    bool isFirstRecord;
    QSqlQuery m_query;

};

#endif // QMLSQLQUERY_H
