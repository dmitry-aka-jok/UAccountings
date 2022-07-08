#include "qmlsqlmodel.h"

#include <QDateTime>

#include "datapipe.h"
#include "qmltypes.h"


QmlSqlModel::QmlSqlModel(QObject *parent) :
    QSortFilterProxyModel(parent), m_serviceColumn(false), m_totalsRow(false), m_appendRow(false), m_filterColumn(-1)

{
    setSourceModel(&sqlmodel);

    connect(this, &QmlSqlModel::filterColumnChanged,  [=](){invalidateRowsFilter();});
    connect(this, &QmlSqlModel::filterStringChanged, [=](){invalidateRowsFilter();});
}

QmlSqlModel::~QmlSqlModel()
{
}


bool QmlSqlModel::exec(const QString &table, QVariantMap queryMap)
{
    Datapipe *datapipe = Datapipe::instance();

    m_fieldsPK.clear();

    QVariantMap tableMap = datapipe->tableDefinition(table);
    foreach (const auto& key,  tableMap.keys()){
        QVariantMap fieldMap = tableMap.value(key).toMap();

        if(fieldMap.value("type").toInt() == SqlType::PK){
            m_fieldsPK.append(key);
        }
    }

    QVariantList fieldsList;
    fieldsList<<m_fieldsPK;

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
    queryMap.insert("fields", fieldsList);



    m_prepQuery = QmlSqlQuery::queryString(table, queryMap);

    if(datapipe->variable("debugQueries", false).toBool())
        qDebug()<<m_prepQuery.query;


    QSqlDatabase db = QSqlDatabase::database();
    sqlmodel.setQuery(m_prepQuery.query, db);
    if ( sqlmodel.lastError().isValid() )
    {
        m_errorString = sqlmodel.lastError().text();
        return false;
    }

    return true;
}

bool QmlSqlModel::reread()
{
    //    sqlmodel.setQuery(m_queryString);

    //    if ( sqlmodel.lastError().isValid() )
    //    {
    //        qCritical()<<"Error in query";
    //        qCritical()<<m_queryString;
    //        qCritical()<<sqlmodel.lastError().text();

    //        m_errorString = sqlmodel.lastError().text();
    //        emit errorOccurred();

    //        return false;
    //    }

    return true;
}


void QmlSqlModel::setSort(int row, Qt::SortOrder order)
{
    //TODO Надо переинициализировать сортировку, если меняется наличие сервис-колонки
    if (row > 0 && m_serviceColumn)
        row--;

    sort(row, order);
}


QHash<int, QByteArray>QmlSqlModel::roleNames() const
{
    QHash<int,QByteArray> hash;

    hash.insert(Qt::UserRole+1, "Display");
    hash.insert(Qt::UserRole+2, "Type");
    hash.insert(Qt::UserRole+3, "Digits");

    return hash;
}

QVariant QmlSqlModel::data(const QModelIndex &index, int role)const
{
    QVariant value;

//    qDebug()<<index.row()<<index.column()<<role<<m_fieldsPK<<(m_serviceColumn?1:0);

    if(role < Qt::UserRole)
    {
        if(m_serviceColumn && index.column()==0)
            value = QString();
        else{
            QModelIndex modelIndex = this->index(index.row(),
                                                 index.column() + m_fieldsPK.count() - (m_serviceColumn?1:0));
            value = QSortFilterProxyModel::data(modelIndex, role);
        }
    }
    else
    {
        if (role==Qt::UserRole+1){ // data
            if(m_serviceColumn && index.column()==0){ // 0-service-column
                value = QString();
            }else{
                int cntAddRows = (m_appendRow?1:0) + (m_totalsRow?1:0);
                if (index.row() >= rowCount()-cntAddRows ){
                    value = QString();
                }else{
                    QModelIndex modelIndex = this->index(index.row(),
                                                         index.column() + m_fieldsPK.count() - (m_serviceColumn?1:0));

                    value = QSortFilterProxyModel::data(modelIndex, Qt::DisplayRole);
                }
            }
        }


        if (role==Qt::UserRole+2){ // types
            if (m_appendRow && index.row() == rowCount()-1){
                if(index.column()==0){
                    value = SqlType::AppendRow;
                }else{
                    value = SqlType::NaT;
                }
            }else{

                if (m_totalsRow && index.row() == rowCount()-1-(m_appendRow?1:0)){
                    value = SqlType::TotalsRow;
                }else{

                    if (m_serviceColumn && index.column() == 0){
                        value = SqlType::ServiceColumn;
                    }else{
                        value = m_prepQuery.types.value(index.column()-(m_serviceColumn?1:0));
                    }
                }
            }
        }


        if (role==Qt::UserRole+3){ //digits
            if(m_serviceColumn && index.column()==0){ // 0-service-column
                value = 0;
            }else{
                value = m_prepQuery.digits.value(index.column()-(m_serviceColumn?1:0));
            }
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
    return QSortFilterProxyModel::rowCount(parent) + (m_appendRow?1:0) + (m_totalsRow?1:0);
}

QVariantMap QmlSqlModel::sqlIndex(int row) const
{
    if(row >= rowCount() - (m_totalsRow?1:0) - (m_appendRow?1:0))
       return QVariantMap();

    QVariantMap res;

    for(int i=0, cnt = m_fieldsPK.count(); i < cnt; i++){
        QModelIndex modelIndex = this->index(row, i);
        res.insert(m_fieldsPK.value(i), QSortFilterProxyModel::data(modelIndex, Qt::DisplayRole));
    }
    return res;
}



bool QmlSqlModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(m_filterString.isEmpty() || m_filterString.isNull())
        return true;

    int appended = (m_appendRow?1:0) + (m_totalsRow?1:0);

    // TODO Подумать - странное условие
    if(appended > 0 && sourceRow >= sourceModel()->rowCount())
         return true;


    int cntPK = m_fieldsPK.count();

    for(int i=0, cnt=columnCount(); i<cnt; i++){
        if(!(m_filterColumn == -1 || m_filterColumn >= cntPK ))
            continue;
        if(!(m_filterColumn == -1 || i == m_filterColumn + cntPK + (m_serviceColumn?1:0)))
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

    SqlType::Value type = m_prepQuery.types.at(left.column());

    if(type==SqlType::Date){
        return sourceModel()->data(leftReal).toDate() < sourceModel()->data(rightReal).toDate();
    }
    if(type==SqlType::Time){
        return sourceModel()->data(leftReal).toTime() < sourceModel()->data(rightReal).toTime();
    }
    if(type==SqlType::DateTime){
        return sourceModel()->data(leftReal).toDateTime() < sourceModel()->data(rightReal).toDateTime();
    }
    if(type==SqlType::PK || type==SqlType::FK || type==SqlType::Int){
        return sourceModel()->data(leftReal).toInt() < sourceModel()->data(rightReal).toInt();
    }
    if(type==SqlType::String){
        return sourceModel()->data(leftReal).toString() < sourceModel()->data(rightReal).toString();
    }
    if(type==SqlType::Bool){
        return sourceModel()->data(leftReal).toBool() < sourceModel()->data(rightReal).toBool();
    }
    if(type==SqlType::Numeric){
        return sourceModel()->data(leftReal).toDouble() < sourceModel()->data(rightReal).toDouble();
    }



    return sourceModel()->data(leftReal).toString() < sourceModel()->data(rightReal).toString();

}
