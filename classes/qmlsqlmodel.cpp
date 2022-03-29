#include "qmlsqlmodel.h"

#include <QDateTime>


QmlDataModel::QmlDataModel(Datapipe *datapipe, QObject *parent) :
    QSortFilterProxyModel(parent), datapipe (datapipe)
{
    setSourceModel(&sqlmodel);
    m_filterColumn = -1;
    setDynamicSortFilter(true);
}


void QmlDataModel::setQueryString(const QString &queryString)
{
    if ( m_queryString == queryString )
        return;

    m_queryString = queryString ;
}


QStringList QmlDataModel::rolesList() const
{
    return m_roleList;
}

QStringList QmlDataModel::typesList() const
{
    return m_typeList;
}

QString QmlDataModel::errorString() const
{
    return m_errorString;
}


bool QmlDataModel::exec()
{
    QSqlDatabase db = QSqlDatabase::database();
    sqlmodel.setQuery(m_queryString, db);

    //qDebug()<<m_queryString;

    if ( sqlmodel.lastError().isValid() )
    {
        qCritical()<<"Error in query";
        qCritical()<<m_queryString;
        qCritical()<<sqlmodel.lastError().text();

        m_errorString = sqlmodel.lastError().text();
        emit errorStringChanged();

        return false;
    }

    m_roleList.clear();
    m_typeList.clear();

    QSqlRecord r = sqlmodel.record();
    for( int i = 0; i < r.count(); i++)
    {
        m_roleList.append(r.fieldName(i).toLatin1());
        m_typeList.append(r.field(i).metaType().name());
    }
    emit rolesListChanged();

    //qDebug()<<sortColumn();
    return true;
}


QHash<int, QByteArray>QmlDataModel::roleNames() const
{
    QHash<int,QByteArray> hash;

    hash.insert(Qt::UserRole+1, "Display"); // QByteArray(record().fieldName(i).toLatin1())
    hash.insert(Qt::UserRole+2, "Type"); // QByteArray(record().fieldName(i).toLatin1())

    return hash;
}

QString QmlDataModel::filterString() const
{
    return m_filterString;
}
int QmlDataModel::filterColumn()
{
    return m_filterColumn;
}

void QmlDataModel::setFilterString(const QString &filterString)
{
    m_filterString = filterString;
    invalidateRowsFilter();
}

void QmlDataModel::setFilterColumn(int filterColumn)
{
    m_filterColumn = filterColumn;
    invalidateRowsFilter();
}

void QmlDataModel::setSort(int row, Qt::SortOrder order)
{
    qDebug()<<row<<order;
    //setSortingEnabled(true);
    sort(row, order);         // responsible call to make sorting, internally it will make a call to lessthan function
//    invalidateFilter();
}


QVariant QmlDataModel::data(const QModelIndex &index, int role)const
{
    QVariant value; // = QSqlQueryModel::data(index, role);

    if(role < Qt::UserRole)
    {
        value = sqlmodel.data(index, role);
    }
    else
    {
        if (role==Qt::UserRole+1)
            value = sqlmodel.data(index, Qt::DisplayRole);
        if (role==Qt::UserRole+2)
            value = m_typeList.value(index.column());
    }

    return value;
}


bool QmlDataModel::lessThan(const QModelIndex &left,
                           const QModelIndex &right) const
{

    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

//    qDebug()<<leftData<<rightData;

    if (leftData.userType() == QMetaType::QDateTime) {
        return leftData.toDateTime() < rightData.toDateTime();
    } else {
        if (leftData.userType() == QMetaType::Int) {
            //qDebug()<<(leftData.toInt() < rightData.toInt());
            return leftData.toInt() < rightData.toInt();
        } else {
            return QString::localeAwareCompare(leftData.toString(), rightData.toString()) < 0;
        }
    }
}

bool QmlDataModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(m_filterColumn < 0)
        return true;
    if(m_filterString.isEmpty() || m_filterString.isNull())
        return true;

    QModelIndex index = sourceModel()->index(sourceRow, m_filterColumn, sourceParent);
    QString data = sourceModel()->data(index, Qt::DisplayRole).value<QString>();

    qDebug()<<sourceRow<<index<<data<<data.contains(m_filterString, Qt::CaseInsensitive);

    if(data.contains(m_filterString, Qt::CaseInsensitive))
        return true;

    return false;
}

QVariant QmlDataModel::value(int row, int column, QVariant deflt) const
{
    QModelIndex modelIndex = sqlmodel.index(row, column);
    QVariant val = sqlmodel.data(modelIndex, Qt::DisplayRole);
    if(val.isValid())
        return val;
    else
        return deflt;
}

QVariant QmlDataModel::value(int row, QString column, QVariant deflt) const
{
    return value(row, m_roleList.indexOf(column), deflt);
}
