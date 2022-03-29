#include "qmlsqltable.h"

#include <QSqlQuery>
#include <QSqlError>

#include "qmlsql.h"

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



QmlDataModel* QmlSqlTable::select(QVariantMap query)
{
    QString queryPrefixs, queryFields, queryJoins, queryWheres, queryLimits, queryOrders, queryHavings;

    QString alias;
    if (query.contains(u"alias"_qs))
        alias = query.value(u"fields"_qs).toString();
    else
        alias = m_table;


    bool isFirst;

    QVariantList fields;
    if (query.contains(u"fields"_qs)){
        // Array or only one field
        if (query.value(u"fields"_qs).typeName() == u"QVariantList"_qs){
            fields = query.value(u"fields"_qs).toList();
        }else{
            fields <<query.value(u"fields"_qs);
        }
    }else{
        // append all fields
        foreach(auto var, m_fields.keys()){
            fields.append(var);
        }
    }
    isFirst=true;
    foreach(auto var, fields){
        QString svar = var.toString();
        if (!svar.contains(u"."_qs))
            svar = alias+u"."_qs+svar;
        if(isFirst)
            isFirst = false;
        else
            svar = u", %1"_qs.arg(svar);

        queryFields +=  svar;
    }


    QVariantList joins;
    if (query.contains(u"joins"_qs)){
        if (query.value(u"joins"_qs).typeName() == u"QVariantList"_qs){
            joins = query.value(u"joins"_qs).toList();
        }else{
            joins <<query.value(u"joins"_qs);
        }
    }
    QStringList tables;
    tables.append(m_table);

    foreach(auto var, joins){
        QString svar = var.toString();
        // variants to use
        //  1. only table name - need to add join, alias, on-condition
        //  ----- 2. table name + alias - need to add join and on-condition
        //  3. full join construction
        if (!svar.contains(u"join"_qs)){
            QString joinTable = svar;
            QString joinOn;

            //qDebug()<<datapipe->table(joinTable);

            foreach (auto tbl, tables){
                QVariantMap tblconf = datapipe->table(tbl);
                foreach (auto fld, tblconf.keys()){ // fields
                    QVariantMap fldconf = tblconf.value(fld).toMap();
                    if (fldconf.contains(u"reference"_qs)){
                        QString ref = fldconf.value(u"reference"_qs).toString();
                        if(ref.startsWith(joinTable+u"."_qs)){
                            joinOn = u" on %1.%2 = %3"_qs.arg(tbl, fld, ref);
                            break;
                        }
                    }
                }
                if(!joinOn.isEmpty()) break;
            }

            if(joinOn.isEmpty()){
                QVariantMap tblconf = datapipe->table(joinTable);
                foreach (auto fld, tblconf.keys()){ // fields
                    QVariantMap fldconf = tblconf.value(fld).toMap();
                    if (fldconf.contains(u"reference"_qs)){
                        foreach (auto tbl, tables){
                            QString ref = fldconf.value(u"reference"_qs).toString();
                            if (ref.startsWith(tbl+u"."_qs)){
                                joinOn = u" on %1.%2 = %3"_qs.arg(joinTable, fld, ref);
                                break;
                            }
                        }
                    }
                    if(!joinOn.isEmpty()) break;
                }
            }

            tables.append(svar);

            svar = u" join %1%2"_qs.arg(svar, joinOn);
        }
        queryJoins +=  svar;//u" %1"_qs.arg(svar);
    }


    QVariantList wheres;
    if (query.contains(u"where"_qs)){
        if (query.value(u"where"_qs).typeName() == u"QVariantMap"_qs){
            QVariantMap map = query.value(u"where"_qs).toMap();
            foreach(auto fld, map.keys()){
                QString fldToCompare = fld;
                if (!fldToCompare.contains(u"."_qs))
                    fldToCompare = u"%1.%2"_qs.arg(alias,fldToCompare);

                wheres<<u"%1=%2"_qs.arg(fldToCompare, QmlSql::formatToSQL(map.value(fld)) );
            }
        }else
        if (query.value(u"where"_qs).typeName() == u"QVariantList"_qs){
            wheres = query.value(u"where"_qs).toList();
        }else{
            wheres <<query.value(u"where"_qs);
        }
    }
    isFirst = true;
    foreach(auto var, wheres){
        QString svar = var.toString();
        QString sand;
        if(isFirst)
            isFirst = false;
        else
            sand = u" and "_qs;

        queryWheres +=  u"%1(%2)"_qs.arg(sand, svar);
    }
    if (!queryWheres.isEmpty())
        queryWheres = u" WHERE %1"_qs.arg(queryWheres);


    if (query.contains(u"limit"_qs))
        queryLimits  = u" LIMIT %1"_qs.arg(query.value(u"limit"_qs).toString());


    QVariantList orders;
    if (query.contains(u"order"_qs)){
        if (query.value(u"order"_qs).typeName() == u"QVariantList"_qs){
            orders = query.value(u"order"_qs).toList();
        }else{
            orders <<query.value(u"order"_qs);
        }
    }
    isFirst = true;
    foreach(auto var, orders){
        QString svar = var.toString();
        QString sand;
        if (!svar.contains(u"."_qs))
            svar = alias+u"."_qs+svar;
        if(isFirst)
            isFirst = false;
        else
            sand = u", "_qs;

        queryOrders +=  u"%1%2"_qs.arg(sand, svar);
    }
    if (!queryOrders.isEmpty())
        queryOrders = u" ORDER BY %1"_qs.arg(queryOrders);


    QVariantList havings;
    if (query.contains(u"having"_qs)){
        if (query.value(u"having"_qs).typeName() == u"QVariantList"_qs){
            havings = query.value(u"having"_qs).toList();
        }else{
            havings <<query.value(u"having"_qs);
        }
    }
    isFirst = true;
    foreach(auto var, havings){
        QString svar = var.toString();
        QString sand = "";
        if(isFirst)
            isFirst = false;
        else
            sand = u", "_qs;

        queryHavings +=  u"%1%2"_qs.arg(sand, svar);
    }
    if (!queryHavings.isEmpty())
        queryHavings = u" HAVING %1"_qs.arg(queryHavings);


    QString queryString = u"SELECT %1%2 FROM %3%4%5%6%7%8%9"_qs
            .arg(queryPrefixs,
                 queryFields,
                 m_table,
                 (alias==m_table?u""_qs:m_table),
                 queryJoins,
                 queryWheres,
                 queryLimits,
                 queryOrders,
                 queryHavings);

    if(datapipe->variable("debugQueries", false).toBool())
        qDebug()<<queryString;

    QmlDataModel* model = new QmlDataModel(datapipe);

    model->setQueryString(queryString);
    model->exec();

   // model->setSortRole(Qt::UserRole+1);
   // model->sort(0, Qt::DescendingOrder);

    return model;
}

// TODO returns
bool QmlSqlTable::insert(QVariantMap fields, QVariantList returns)
{
    Q_UNUSED(returns)

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




bool QmlSqlTable::insert_or_update(QVariantMap fields, QVariantMap conditions)
{
    if (fields.isEmpty() || conditions.isEmpty()){
        m_lastError = u"Empty query"_qs;
        return false;
    }

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
