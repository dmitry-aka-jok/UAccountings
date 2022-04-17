#include "qmlsql.h"


#include <QDebug>
#include <QVariant>

QmlSql::QmlSql(QObject *parent)
    : QObject{parent}
{
    datapipe = Datapipe::instance();
}

void QmlSql::defineTable(QString name, QVariantMap fields)
{
    datapipe->addTable(name, fields);
}

QmlSqlTable *QmlSql::table(QString name)
{
    QmlSqlTable *qmltable = new QmlSqlTable();
    qmltable->setTable(name);
    qmltable->setFields(datapipe->table(name));

    //qDebug()<<datapipe->tables()<<name<<datapipe->table(name);

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
