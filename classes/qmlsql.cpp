#include "qmlsql.h"


#include <QDebug>
#include <QVariant>

QmlSql::QmlSql(Datapipe *datapipe, QObject *parent)
    : QObject{parent}, datapipe(datapipe)
{

}

QmlSqlTable *QmlSql::table(QString name, QVariantMap fields)
{
    QmlSqlTable *qmltable = new QmlSqlTable(datapipe);
    qmltable->setTable(name);
    qmltable->setFields(fields);

    if (!qmltable->validate())
    {
        qCritical()<<u"Error validating table"_qs<<name;
        qCritical()<<qmltable->lastError();
    }

    return qmltable;
}
