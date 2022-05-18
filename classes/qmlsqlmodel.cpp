#include "qmlsqlmodel.h"

#include <QDateTime>

#include "datapipe.h"
#include "qmlsql.h"


QmlSqlModel::QmlSqlModel(QObject *parent) :
    QSortFilterProxyModel(parent), m_serviceColumn(false), m_totalsRow(false), m_appendRow(false), m_filterColumn(-1)

{
    setSourceModel(&sqlmodel);
}

QmlSqlModel::~QmlSqlModel()
{
}


QStringList QmlSqlModel::rolesList() const
{
    return m_roleList;
}

QStringList QmlSqlModel::typesList() const
{
    return m_typeList;
}

QString QmlSqlModel::errorString() const
{
    return m_errorString;
}

bool QmlSqlModel::exec(const QString &table, const QVariantMap &fields, const QVariantMap &query)
{
    m_query  = query;
    m_table  = table;
    m_fields = fields;

    Datapipe *datapipe = Datapipe::instance();

    QString queryPrefixs, queryFields, queryJoins, queryWheres, queryLimits, queryOrders, queryHavings;

    QString alias;
    if (query.contains(u"alias"_qs))
        alias = query.value(u"fields"_qs).toString();
    else
        alias = table;

    QMap<QString, QString> aliases;
    aliases.insert(table, alias);

    bool isFirst;

    m_fieldsPK.clear();
    QVariantList fieldsList;
    foreach(auto var, fields.keys()){
        QVariantMap vmap = fields.value(var).toMap();
        if(vmap.value(u"type"_qs).toInt()==QmlSql::PK){
            m_fieldsPK.insert(var, vmap);
            fieldsList<<var;
        }
    }




    // joins first - to get right aliases
    QVariantList joins;
    if (query.contains(u"joins"_qs)){
        if (query.value(u"joins"_qs).typeName() == u"QVariantList"_qs){
            joins = query.value(u"joins"_qs).toList();
        }else{
            joins <<query.value(u"joins"_qs);
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
            QRegularExpressionMatch match = QRegularExpression("join (\\w+) (\\w+)").match(svar);
            if (match.hasMatch()) {
                QString joinTable = match.captured(1);
                QString joinAlias = match.captured(2);
                aliases.insert(joinAlias, joinTable);

            }
        }


        queryJoins += u" "_qs + svar;
    }



    m_typeList.clear();
    m_digitsList.clear();


    if (query.contains(u"fields"_qs)){
        // Array or only one field
        if (query.value(u"fields"_qs).typeName() == u"QVariantList"_qs){
            fieldsList<<query.value(u"fields"_qs).toList();
        }else{
            fieldsList<<query.value(u"fields"_qs);
        }
    }else{
        // append all fields
        foreach(auto var, fields.keys()){
            fieldsList.append(var);
        }
    }
    isFirst=true;

    int cnt = 0;
    foreach(auto var, fieldsList){
        QString svar = var.toString();
        if (!svar.contains(u"."_qs))
            svar = u"%1.%2"_qs.arg(alias, svar);

        QString currAlias = svar.left(svar.indexOf('.'));
        QString currField = svar.mid(svar.indexOf('.')+1);
        QVariantMap fieldDescr = datapipe->tableDefinition(aliases.value(currAlias)).value(currField).toMap();

        QmlSql::FieldType fieldType = static_cast<QmlSql::FieldType>(fieldDescr.value(u"type"_qs).toInt());

        if(cnt>=m_fieldsPK.count()){ // skip PKs
            m_digitsList.append(fieldDescr.value(u"digits"_qs,0).toInt());

            if(fieldType==QmlSql::PK || fieldType==QmlSql::FK || fieldType==QmlSql::Int){
                m_typeList.append(u"Int"_qs);
            }else{
                if(fieldType==QmlSql::Bool){
                    m_typeList.append(u"Bool"_qs);
                }else{
                    if(fieldType==QmlSql::String){
                        m_typeList.append(u"String"_qs);
                    }else{
                        if(fieldType==QmlSql::Numeric){
                            m_typeList.append(u"Numeric"_qs);
                        }else{
                            m_typeList.append(u"String"_qs);
                        }
                    }
                }
            }
        }


        if(isFirst)
            isFirst = false;
        else
            svar = u", %1"_qs.arg(svar);

        queryFields +=  svar;
        cnt++;
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


    m_queryString = u"SELECT %1%2 FROM %3%4%5%6%7%8%9"_qs
            .arg(queryPrefixs,
                 queryFields,
                 table,
                 (alias==table?u""_qs:table),
                 queryJoins,
                 queryWheres,
                 queryLimits,
                 queryOrders,
                 queryHavings);


    if(datapipe->variable("debugQueries", false).toBool())
        qDebug()<<m_queryString;


    QSqlDatabase db = QSqlDatabase::database();
    sqlmodel.setQuery(m_queryString, db);

    if ( sqlmodel.lastError().isValid() )
    {
        qCritical()<<"Error in query";
        qCritical()<<m_queryString;
        qCritical()<<sqlmodel.lastError().text();

        m_errorString = sqlmodel.lastError().text();
        emit errorOccurred();

        return false;
    }

    m_roleList.clear();
    //    m_typeList.clear();

    QSqlRecord r = sqlmodel.record();
    // skip shadow PK fields
    for( int i = m_fieldsPK.count(); i < r.count(); i++)
    {
        m_roleList.append(r.fieldName(i).toLatin1());
        //        m_typeList.append(r.field(i).metaType().name());
    }
    emit rolesListChanged();

    return true;
}

bool QmlSqlModel::reread()
{
    if(m_queryString.isEmpty())
        return false;

    sqlmodel.setQuery(m_queryString);

    if ( sqlmodel.lastError().isValid() )
    {
        qCritical()<<"Error in query";
        qCritical()<<m_queryString;
        qCritical()<<sqlmodel.lastError().text();

        m_errorString = sqlmodel.lastError().text();
        emit errorOccurred();

        return false;
    }

    return true;
}

QString QmlSqlModel::filterString() const
{
    return m_filterString;
}
int QmlSqlModel::filterColumn()
{
    return m_filterColumn;
}

void QmlSqlModel::setFilterString(const QString &filterString)
{
    m_filterString = filterString;
    invalidateRowsFilter();
}

void QmlSqlModel::setFilterColumn(int filterColumn)
{
    m_filterColumn = filterColumn;
    invalidateRowsFilter();
}

void QmlSqlModel::setSort(int row, Qt::SortOrder order)
{
    sort(row, order);
}

void QmlSqlModel::setServiceColumn(bool serviceColumn)
{
    if(m_serviceColumn == serviceColumn)
        return;

    m_serviceColumn = serviceColumn;
    emit serviceColumnChanged();
}

bool QmlSqlModel::serviceColumn()
{
    return m_serviceColumn;
}

void QmlSqlModel::setTotalsRow(bool totalsRow)
{
    if(m_totalsRow == totalsRow)
        return;

    m_totalsRow = totalsRow;
    emit totalsRowChanged();
}

bool QmlSqlModel::totalsRow()
{
    return m_totalsRow;
}

void QmlSqlModel::setAppendRow(bool appendRow)
{
    if(m_appendRow == appendRow)
        return;

    m_appendRow = appendRow;
    emit appendRowChanged();
}

bool QmlSqlModel::appendRow()
{
    return m_appendRow;
}

//QModelIndex QmlSqlModel::index(int row, int column, const QModelIndex& parent) const {
//    return createIndex(row,column,row);
//}

//QModelIndex QmlSqlModel::parent(const QModelIndex &index) const {
//    //Works only for non-tree models
//    return QModelIndex();
//}

//QModelIndex QmlSqlModel::mapFromSource(const QModelIndex &source) const {
//    return index(source.row(), source.column(), source.parent());
//}

//QModelIndex QmlSqlModel::mapToSource(const QModelIndex &proxy) const {
//    return (sourceModel()&&proxy.isValid())
//            ? sourceModel()->index(proxy.row(), proxy.column(), proxy.parent())
//            : QModelIndex();
//}


QHash<int, QByteArray>QmlSqlModel::roleNames() const
{
    QHash<int,QByteArray> hash;

    hash.insert(Qt::UserRole+1, "Display");
    hash.insert(Qt::UserRole+2, "Type");
    hash.insert(Qt::UserRole+3, "Digits");

//    qDebug()<<hash;

    return hash;
}

QVariant QmlSqlModel::data(const QModelIndex &index, int role)const
{
    QVariant value;
//    qDebug()<<"data"<<index.row()<<index.column()<<role;

    if(role < Qt::UserRole)
    {
        if(m_serviceColumn && index.column()==0) // 0-service-column
            return QString();

        QModelIndex modelIndex = this->index( index.row(), index.column() + m_fieldsPK.count() - (m_serviceColumn?1:0));
        value = QSortFilterProxyModel::data(modelIndex, role);
    }
    else
    {
        if (role==Qt::UserRole+1){ // data
            if(m_serviceColumn && index.column()==0) // 0-service-column
                return QString();
            if (index.row() == rowCount()-1 && m_appendRow)
                return QString();

            QModelIndex modelIndex = this->index( index.row(), index.column() + m_fieldsPK.count() - (m_serviceColumn?1:0));
            value = QSortFilterProxyModel::data(modelIndex, Qt::DisplayRole);
        }
        if (role==Qt::UserRole+2){ // types
            if (index.row() == rowCount()-1 && m_appendRow && index.column()==0)
                return QString(u"Add"_qs);
            if (index.row() == rowCount()-1 && m_appendRow && index.column()!=0)
                return QString(u"Empty"_qs);
            if (index.row() == rowCount()-1 && !m_appendRow && m_totalsRow)
                return QString(u"Totals"_qs);
            if (index.row() == rowCount()-2 && m_appendRow && m_totalsRow)
                return QString(u"Totals"_qs);
            // after rows -
            if(m_serviceColumn && index.column()==0) // 0-service-column
                return QString(u"Service"_qs);


            value = m_typeList.value(index.column()-(m_serviceColumn?1:0));
        }
        if (role==Qt::UserRole+3){ //digits
            if(m_serviceColumn && index.column()==0) // 0-service-column
                return 0;

            value = m_digitsList.value(index.column()-(m_serviceColumn?1:0));
        }
    }

    return value;
}

int QmlSqlModel::columnCount(const QModelIndex &parent) const
{
    return QSortFilterProxyModel::columnCount(parent) - m_fieldsPK.count() + (m_serviceColumn?1:0);
}

int QmlSqlModel::rowCount(const QModelIndex &parent) const
{
//    qDebug()<<(QSortFilterProxyModel::rowCount(parent) + (m_appendRow?1:0) + (m_totalsRow?1:0));
    return QSortFilterProxyModel::rowCount(parent) + (m_appendRow?1:0) + (m_totalsRow?1:0);
}

QVariantMap QmlSqlModel::sqlIndex(int row) const
{
    QStringList fieldsPK;
    fieldsPK<<m_fieldsPK.keys();
    QVariantMap res;

    for(int i=0, cnt = m_fieldsPK.count(); i < cnt; i++){
        QModelIndex modelIndex = this->index( row, i);
        res.insert(fieldsPK.at(i), QSortFilterProxyModel::data(modelIndex, Qt::DisplayRole));
    }
    return res;
}



bool QmlSqlModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(m_filterString.isEmpty() || m_filterString.isNull())
        return true;

    int appended = (m_appendRow?1:0) - (m_totalsRow?1:0);

    if(appended > 0 && sourceRow >= (rowCount() - appended) )
       return true;


    int cntPK = m_fieldsPK.count();

    for(int i=0, cnt=columnCount(); i<cnt; i++){
        if(!(m_filterColumn == -1 || m_filterColumn + cntPK + (m_serviceColumn?1:0) == i))
            continue;

        QModelIndex index = sourceModel()->index(sourceRow, i-(m_serviceColumn?1:0), sourceParent);
        QString data = sourceModel()->data(index, Qt::DisplayRole).value<QString>();

        if(data.contains(m_filterString, Qt::CaseInsensitive))
            return true;
    }

    return false;
}

bool QmlSqlModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QModelIndex leftReal = sourceModel()->index(left.row(), left.column() + m_fieldsPK.count());
    QModelIndex rightReal = sourceModel()->index(right.row(), right.column() + m_fieldsPK.count());

    QString type = m_typeList.at(left.column());

    if(type==u"Numeric"_qs){
        return sourceModel()->data(leftReal).toDouble() < sourceModel()->data(rightReal).toDouble();
    }
    if(type==u"String"_qs){
        return sourceModel()->data(leftReal).toString() < sourceModel()->data(rightReal).toString();
    }
    if(type==u"Bool"_qs){
        return sourceModel()->data(leftReal).toBool() < sourceModel()->data(rightReal).toBool();
    }


    return sourceModel()->data(leftReal).toString() < sourceModel()->data(rightReal).toString();

}

QVariant QmlSqlModel::value(int row, int column, QVariant deflt) const
{
//    if(sqlmodel.columnCount()<(column+1) || sqlmodel.rowCount() < (row+1))
//        return deflt;

//    qDebug()<<column<<m_fieldsPK.count()<<-(m_serviceColumn?1:0);
//    QSortFilterProxyModel::data(modelIndex, Qt::DisplayRole)

    QModelIndex modelIndex = QSortFilterProxyModel::index(row, column + m_fieldsPK.count() - (m_serviceColumn?1:0));
    QVariant val = QSortFilterProxyModel::data(modelIndex, Qt::DisplayRole);

    if(val.isValid())
        return val;
    else
        return deflt;
}

QVariant QmlSqlModel::value(int row, QString column, QVariant deflt) const
{
    int clmn = m_roleList.indexOf(column);
    if(clmn>=0)
        return value(row, clmn, deflt);
    else
        return deflt;
}
