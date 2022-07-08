#include "qmlsqlquery.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "datapipe.h"
#include "qmltypes.h"


QmlSqlQuery::QmlSqlQuery(QObject *parent)
{
}

QmlSqlQuery::~QmlSqlQuery()
{
}

PrepearedQuery QmlSqlQuery::queryString(const QString &table, QVariantMap queryMap)
{
    PrepearedQuery result;

    Datapipe *datapipe = Datapipe::instance();

    QString queryPrefixs, queryFields, queryJoins, queryWheres, queryLimits, queryOrders, queryHavings;

    QString alias;
    if (queryMap.contains(u"alias"_qs))
        alias = queryMap.value(u"fields"_qs).toString();
    else
        alias = table;

    QMap<QString, QString> aliases;
    aliases.insert(table, alias);

    bool isFirst;

    QVariantList fieldsList;


    // joins first - to get right aliases
    QVariantList joins;
    if (queryMap.contains(u"joins"_qs)){
        if (queryMap.value(u"joins"_qs).typeName() == u"QVariantList"_qs){
            joins = queryMap.value(u"joins"_qs).toList();
        }else{
            joins <<queryMap.value(u"joins"_qs);
        }
    }
    QStringList tables;
    tables.append(table);

    foreach(auto var, joins){
        QString svar = var.toString();
        // variants to use
        //  1. only table name - need to add join, alias, on-condition
        //  ----- 2. table name + alias - need to add join and on-condition
        //  3. full join construction
        if (!svar.contains(u"join"_qs)){
            QString joinTable = svar;
            QString joinOn;

            aliases.insert(joinTable, joinTable);

            foreach (auto tbl, tables){
                QVariantMap tblconf = datapipe->tableDefinition(tbl);
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
                QVariantMap tblconf = datapipe->tableDefinition(joinTable);
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

            svar = u"join %1%2"_qs.arg(svar, joinOn);
        }
        else
        {
            static QRegularExpression re("join (\\w+) (\\w+)");
            QRegularExpressionMatch match = re.match(svar);
            if (match.hasMatch()) {
                QString joinTable = match.captured(1);
                QString joinAlias = match.captured(2);
                aliases.insert(joinAlias, joinTable);

            }
        }


        queryJoins += u" "_qs + svar;
    }


    if (queryMap.contains(u"fields"_qs)){
        // Array or only one field
        if (queryMap.value(u"fields"_qs).typeName() == u"QVariantList"_qs){
            fieldsList<<queryMap.value(u"fields"_qs).toList();
        }else{
            fieldsList<<queryMap.value(u"fields"_qs);
        }
    }else{
        // append all fields
        QVariantMap tblconf = datapipe->tableDefinition(table);
        foreach (auto fld, tblconf.keys()) // fields
            fieldsList.append(fld);
    }

    isFirst=true;

    foreach(auto var, fieldsList){
        QString svar = var.toString();
        if (!svar.contains(u"."_qs))
            svar = u"%1.%2"_qs.arg(alias, svar);

        QString currAlias = svar.left(svar.indexOf('.'));
        QString currField = svar.mid(svar.indexOf('.')+1);
        QVariantMap fieldDescr = datapipe->tableDefinition(aliases.value(currAlias)).value(currField).toMap();

        result.types<<static_cast<SqlType::Value>(fieldDescr.value(u"type"_qs).toInt());
        result.digits<<fieldDescr.value(u"digits"_qs,0).toInt();

        if(isFirst)
            isFirst = false;
        else
            svar = u", %1"_qs.arg(svar);

        queryFields +=  svar;
    }


    QVariantList wheres;
    if (queryMap.contains(u"where"_qs)){
        if (queryMap.value(u"where"_qs).typeName() == u"QVariantMap"_qs){
            QVariantMap map = queryMap.value(u"where"_qs).toMap();
            foreach(auto fld, map.keys()){
                QString fldToCompare = fld;
                QString fldTable;

                if (fldToCompare.contains(u"."_qs)){
                    QString joinAlias = fldToCompare.left(fldToCompare.indexOf(u"."_qs));
                    fldTable = aliases.value(joinAlias);
                }else{
                    fldToCompare = u"%1.%2"_qs.arg(alias,fldToCompare);
                    fldTable = table;
                }

                QVariantMap tblconf = datapipe->tableDefinition(fldTable);
                QVariantMap fldMap = tblconf.value(fld).toMap();

                SqlType::Value type = static_cast<SqlType::Value> (fldMap.value("type").toInt());

                //qDebug()<<tblconf<<Qt::endl<<fld<<fldMap<<type;
                wheres<<u"%1=%2"_qs.arg(fldToCompare, Datapipe::formatToSQL(map.value(fld), type) );
            }
        }else
            if (queryMap.value(u"where"_qs).typeName() == u"QVariantList"_qs){
                wheres = queryMap.value(u"where"_qs).toList();
            }else{
                wheres <<queryMap.value(u"where"_qs);
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


    if (queryMap.contains(u"limit"_qs))
        queryLimits  = u" LIMIT %1"_qs.arg(queryMap.value(u"limit"_qs).toString());


    QVariantList orders;
    if (queryMap.contains(u"order"_qs)){
        if (queryMap.value(u"order"_qs).typeName() == u"QVariantList"_qs){
            orders = queryMap.value(u"order"_qs).toList();
        }else{
            orders <<queryMap.value(u"order"_qs);
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
    if (queryMap.contains(u"having"_qs)){
        if (queryMap.value(u"having"_qs).typeName() == u"QVariantList"_qs){
            havings = queryMap.value(u"having"_qs).toList();
        }else{
            havings <<queryMap.value(u"having"_qs);
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


    result.query = u"SELECT %1%2 FROM %3%4%5%6%7%8%9"_qs
            .arg(queryPrefixs,
                 queryFields,
                 table,
                 (alias==table?u""_qs:table),
                 queryJoins,
                 queryWheres,
                 queryLimits,
                 queryOrders,
                 queryHavings);



    return result;
}

bool QmlSqlQuery::exec(const QString &table, QVariantMap queryMap)
{
    Datapipe *datapipe = Datapipe::instance();

    QString m_queryString = QmlSqlQuery::queryString(table, queryMap).query;

    if(datapipe->variable("debugQueries", false).toBool())
        qDebug()<<m_queryString;

    isFirstRecord = true;
    return m_query.exec(m_queryString);
}

bool QmlSqlQuery::next()
{
    if(isFirstRecord)
        isFirstRecord = false;
    return m_query.next();
}

QVariant QmlSqlQuery::value(int index, QVariant deflt)
{
    bool isValid = true;

    if(isFirstRecord){
        isValid = next();
    }

    if(isValid)
       return m_query.value(index);
    else
       return deflt;
}

QVariant QmlSqlQuery::value(const QString &name, QVariant deflt)
{
    bool isValid = true;

    if(isFirstRecord){
        isValid = next();
    }

    if(isValid)
       return m_query.value(name);
    else
       return deflt;
}

bool QmlSqlQuery::isValid() const
{
    return m_query.isValid();
}


