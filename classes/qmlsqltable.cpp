#include "qmlsqltable.h"

#include <QSqlQuery>
#include <QSqlError>

QmlSqlTable::QmlSqlTable(Datapipe *datapipe, QObject *parent)
    : QObject{parent}, datapipe(datapipe)
{

}

void QmlSqlTable::setTable(QString table)
{
    m_table = table;
    emit tableChanged();
}

void QmlSqlTable::setFields(QVariantMap fields)
{
    m_fields = fields;
}

QString QmlSqlTable::table()
{
    return m_table;
}

QVariantMap QmlSqlTable::fields()
{
    return m_fields;
}

bool QmlSqlTable::validate()
{
    m_lastError.clear();

    if(m_table.isEmpty()){
        m_lastError = u"Can't validate with empty table name"_qs;
        return false;
    }
    if(m_fields.isEmpty()){
        m_lastError = u"Can't validate with empty table fields"_qs;
        return false;
    }

    if(datapipe->settings("database/driver").toString()==u"QPSQL"_qs){
        QSqlQuery query;

        QString checkTable = u"SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME='%1'"_qs.arg(m_table);
        if(!query.exec(checkTable)){
            m_lastError = query.lastError().text();
            return false;
        }
        query.next();
        if (!query.isValid()){
            //qDebug()<<"Create";
            // СОЗДАЕМ НОВУЮ
        }else{
            //qDebug()<<"Check modify";
            // Надо сверять и пересоздавать
        }

    }

    return true;
}

QString QmlSqlTable::lastError()
{
    return m_lastError;
}
