#include "qmlsqltable.h"

#include <QSqlQuery>
#include <QSqlError>

#include "datapipe.h"

QmlSqlTable::QmlSqlTable(QObject *parent)
    : QObject{parent}
{
}

void QmlSqlTable::setTable(const QString &table)
{
    m_table = table;
    emit tableChanged();
}

void QmlSqlTable::setFields(const QVariantMap &fields)
{
    m_fields = fields;
    emit fieldsChanged();
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
    Datapipe *datapipe = Datapipe::instance();

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



QmlSqlModel* QmlSqlTable::select(QVariantMap query)
{
    QmlSqlModel* model = new QmlSqlModel();

    model->exec(m_table, m_fields, query);

    return model;
}

// TODO returns
bool QmlSqlTable::insert(QVariantMap fields, QVariantList returns)
{
    Q_UNUSED(returns)

    Datapipe *datapipe = Datapipe::instance();

    if (fields.isEmpty()){
        m_lastError = u"Empty query"_qs;
        return false;
    }

    QStringList fieldsFullHolder;
    fieldsFullHolder.fill("?",fields.count());

    QString queryString = u"INSERT INTO %1 (%2) VALUES (%3)"_qs
            .arg(m_table, fields.keys().join(','), fieldsFullHolder.join(','));

    /*
    if (!returns.isEmpty()){
        bool isFirst = true;
        queryString += u" returning ("_qs;
        foreach(auto key, returns){
            if(!isFirst)
                queryString += u","_qs;
            else
                isFirst = false;

            queryString += key.toString();
        }
        queryString += u")"_qs;
    }
    */

    QSqlQuery query;
    query.prepare(queryString);
    foreach(auto key, fields.keys())
        query.addBindValue(fields.value(key));

    if(datapipe->variable("debugQueries", false).toBool()){
        qDebug()<<queryString;
        qDebug()<<query.boundValues();
    }

    if(!query.exec()){
        m_lastError = query.lastError().text();

        qCritical()<<m_lastError;
        qCritical()<<u"Query:"_qs<<query.lastQuery();
        qCritical()<<u"Values:"_qs<<query.boundValues();

        return false;
    }

    return true;
}


bool QmlSqlTable::update(QVariantMap fields, QVariantMap conditions)
{
    if (fields.isEmpty() || conditions.isEmpty()){
        m_lastError = u"Empty query"_qs;
        return false;
    }

    Datapipe *datapipe = Datapipe::instance();

    QStringList fieldsUpdate;
    foreach(auto key, fields.keys()){
        fieldsUpdate.append(u"%1 = ?"_qs.arg(key));
    }
    QStringList condUpdate;
    foreach(auto key, conditions.keys()){
        condUpdate.append(u"%1 = ?"_qs.arg(key));
    }

    QString queryString = u"UPDATE %1 SET %2 WHERE %3"_qs
            .arg(m_table, fieldsUpdate.join(','), condUpdate.join(" and "));

    QSqlQuery query;
    query.prepare(queryString);
    foreach(auto key, fields.keys())
        query.addBindValue(fields.value(key));
    foreach(auto key, conditions.keys())
        query.addBindValue(conditions.value(key));

    if(datapipe->variable("debugQueries", false).toBool()){
        qDebug()<<queryString;
        qDebug()<<query.boundValues();
    }

    if(!query.exec()){
        m_lastError = query.lastError().text();

        qCritical()<<m_lastError;
        qCritical()<<u"Query:"_qs<<query.lastQuery();
        qCritical()<<u"Values:"_qs<<query.boundValues();

        return false;
    }

    return true;
}

bool QmlSqlTable::update(QVariantMap fields, QString conditions)
{
    // can't merge updates becouse of 2 bindings condition values

    if (fields.isEmpty() || conditions.isEmpty()){
        m_lastError = u"Empty query"_qs;
        return false;
    }

    Datapipe *datapipe = Datapipe::instance();

    QStringList fieldsUpdate;
    foreach(auto key, fields.keys()){
        fieldsUpdate.append(u"%1 = ?"_qs.arg(key));
    }

    QString queryString = u"UPDATE %1 SET %2 WHERE %3"_qs
            .arg(m_table, fieldsUpdate.join(','), conditions);

    QSqlQuery query;
    query.prepare(queryString);
    foreach(auto key, fields.keys())
        query.addBindValue(fields.value(key));

    if(datapipe->variable("debugQueries", false).toBool()){
        qDebug()<<queryString;
        qDebug()<<query.boundValues();
    }

    if(!query.exec()){
        m_lastError = query.lastError().text();

        qCritical()<<m_lastError;
        qCritical()<<u"Query:"_qs<<query.lastQuery();
        qCritical()<<u"Values:"_qs<<query.boundValues();

        return false;
    }

    return true;

}




bool QmlSqlTable::insert_or_update(QVariantMap fields, QVariantMap conditions)
{
    if (fields.isEmpty() || conditions.isEmpty()){
        m_lastError = u"Empty query"_qs;
        return false;
    }

    Datapipe *datapipe = Datapipe::instance();

    QStringList fieldsFullHolder;
    fieldsFullHolder.fill("?",fields.count()+conditions.count());

    QStringList fieldsUpdate;
    foreach(auto key, fields.keys()){
        fieldsUpdate.append(u"%1 = ?"_qs.arg(key));
    }

    QString queryString = u"INSERT INTO %1 (%2,%3) VALUES (%4) ON CONFLICT (%5) DO UPDATE SET %6"_qs
            .arg(m_table,
                 fields.keys().join(','),
                 conditions.keys().join(','),
                 fieldsFullHolder.join(','),
                 conditions.keys().join(','),
                 fieldsUpdate.join(','));

    QSqlQuery query;
    query.prepare(queryString);
    foreach(auto key, fields.keys())
        query.addBindValue(fields.value(key));
    foreach(auto key, conditions.keys())
        query.addBindValue(conditions.value(key));
    foreach(auto key, fields.keys())
        query.addBindValue(fields.value(key));

    if(datapipe->variable("debugQueries", false).toBool()){
        qDebug()<<queryString;
        qDebug()<<query.boundValues();
    }

    if(!query.exec()){
        m_lastError = query.lastError().text();

        qCritical()<<m_lastError;
        qCritical()<<u"Query:"_qs<<query.lastQuery();
        qCritical()<<u"Values:"_qs<<query.boundValues();

        return false;
    }

    return true;
}



bool QmlSqlTable::remove(QVariantMap fields)
{
    if (fields.isEmpty()){
        m_lastError = u"Empty query"_qs;
        return false;
    }

    Datapipe *datapipe = Datapipe::instance();

    QStringList fieldsRemove;
    foreach(auto key, fields.keys()){
        fieldsRemove.append(u"%1 = ?"_qs.arg(key));
    }

    QString queryString = u"DELETE FROM %1 WHERE %2"_qs.arg(m_table, fieldsRemove.join(" and "));

    QSqlQuery query;
    query.prepare(queryString);
    foreach(auto key, fields.keys())
        query.addBindValue(fields.value(key));

    if(datapipe->variable("debugQueries", false).toBool()){
        qDebug()<<queryString;
        qDebug()<<query.boundValues();
    }

    if(!query.exec()){
        m_lastError = query.lastError().text();

        qCritical()<<m_lastError;
        qCritical()<<u"Query:"_qs<<query.lastQuery();
        qCritical()<<u"Values:"_qs<<query.boundValues();

        return false;
    }

    return true;
}



QString QmlSqlTable::lastError()
{
    return m_lastError;
}
