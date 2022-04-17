#include "qmlsqlmodel.h"

#include <QDateTime>


QmlSqlModel::QmlSqlModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    datapipe = Datapipe::instance();
    setSourceModel(&sqlmodel);
    m_filterColumn = -1;
}

QmlSqlModel::~QmlSqlModel()
{
//   qDebug()<<"Destroy model"<<m_queryString;
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


bool QmlSqlModel::exec(const QString &queryString)
{
    m_queryString  = queryString;

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
    m_typeList.clear();

    QSqlRecord r = sqlmodel.record();
    for( int i = 0; i < r.count(); i++)
    {
        m_roleList.append(r.fieldName(i).toLatin1());
        m_typeList.append(r.field(i).metaType().name());
    }
    emit rolesListChanged();

    return true;
}


QHash<int, QByteArray>QmlSqlModel::roleNames() const
{
    QHash<int,QByteArray> hash;

    hash.insert(Qt::UserRole+1, "Display"); // QByteArray(record().fieldName(i).toLatin1())
    hash.insert(Qt::UserRole+2, "Type"); // QByteArray(record().fieldName(i).toLatin1())

    return hash;
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


QVariant QmlSqlModel::data(const QModelIndex &index, int role)const
{
    QVariant value;

    if(role < Qt::UserRole)
    {
        value = QSortFilterProxyModel::data(index, role);
    }
    else
    {
        if (role==Qt::UserRole+1)
            value = QSortFilterProxyModel::data(index, Qt::DisplayRole);
        if (role==Qt::UserRole+2)
            value = m_typeList.value(index.column());
    }

    return value;
}



bool QmlSqlModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    //    if(m_filterColumn < -1)
    //        return true;
    if(m_filterString.isEmpty() || m_filterString.isNull())
        return true;


    for(int i=columnCount()-1;i>=0;i--){
        if(!(m_filterColumn == -1 || m_filterColumn == i))
            continue;

        QModelIndex index = sourceModel()->index(sourceRow, i, sourceParent);
        QString data = sourceModel()->data(index, Qt::DisplayRole).value<QString>();

        if(data.contains(m_filterString, Qt::CaseInsensitive))
            return true;
    }

    return false;
}

QVariant QmlSqlModel::value(int row, int column, QVariant deflt) const
{
    QModelIndex modelIndex = sqlmodel.index(row, column);
    QVariant val = sqlmodel.data(modelIndex, Qt::DisplayRole);
    if(val.isValid())
        return val;
    else
        return deflt;
}

QVariant QmlSqlModel::value(int row, QString column, QVariant deflt) const
{
    return value(row, m_roleList.indexOf(column), deflt);
}
