#include "qmlsql.h"


#include <QDebug>
#include <QVariant>

QmlSql::QmlSql(Datapipe *datapipe, QObject *parent)
    : QObject{parent}, datapipe(datapipe)
{

}

void QmlSql::defineTable(QString name, QVariantMap fields)
{
    //QmlSqlTable *qmltable = new QmlSqlTable(datapipe);
    //qmltable->setTable(name);
    //qmltable->setFields(fields);

    datapipe->addTable(name, fields);
    //qDebug()<<name<<fields;

    // validate will be actual later
    //if (!qmltable->validate()){
    //    qCritical()<<u"Error validating table"_qs<<name;
    //    qCritical()<<qmltable->lastError();
    //}

    //return qmltable;
}

QmlSqlTable *QmlSql::table(QString name)
{
    QmlSqlTable *qmltable = new QmlSqlTable(datapipe);
    qmltable->setTable(name);
    qmltable->setFields(datapipe->table(name));

    return qmltable;
}

QString QmlSql::formatToSQL(QVariant val)
{
    QString type = val.metaType().name();
    if (type==u"QString"_qs)
        return u"'%1'"_qs.arg(val.toString());
    if (type==u"QDate"_qs || type==u"QDateTime"_qs || type==u"QTime"_qs)
        return u"'%1'"_qs.arg(val.toString());

    return val.toString();
}
